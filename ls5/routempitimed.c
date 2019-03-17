/* routempi.c 
  parralel implementation of route.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include "mpi.h"

#define GENERIC_MAX(x, y) ((x) > (y) ? (x) : (y))
#define GENERIC_MIN(x, y) ((x) < (y) ? (x) : (y))

#define MAX_POS 10000

double  lat[MAX_POS];
double  lon[MAX_POS];
double elev[MAX_POS];
double dist[MAX_POS];
double incr[MAX_POS];
double gdist[MAX_POS];
double gincr[MAX_POS];

int positions = 0;

int bitesize;

int i,j;

/* Identity info (not used in this version) */
#define MASTER 0
int id, procs;

int first, last;
double dist_to_send;
double recvbuf;

//Length
double global_length;
double global_max_elev, global_min_elev;
double global_max_slope, global_min_slope;

//extremes
double local_max_elev, local_min_elev;
double local_max_slope, local_min_slope;

//ascent
double global_longest_ascent;

double local_start_ascent;
double local_mid_ascent;
double local_end_ascent;

double start_ascents[0x1000];
double mid_ascents[0x1000];
double end_ascents[0x1000];
double combined_ascents[0x1000];

/* Height median */
int global_height_median;

#define PI 3.14159265358979323846
#define R1 6371009                   // Earth's  middle radius (m) 

/* Not to be changed */
void read_route(char * name){
  char line[100];
  int pos = 0;

  FILE * data = fopen(name,"r");

  if (data==NULL) {
    printf("Error, file not found: %s\n", name);
    exit(1);
  }

  while (fgets(line, sizeof(line), data) != NULL) {
    sscanf(line, "%lf %lf %lf", &lat[pos], &lon[pos], &elev[pos]);
    pos++;
    if (pos == MAX_POS) {
      printf("Error: Route file too big\n");
      exit(1);
    }
  }  

  positions = pos;
}

/* Not to be changed */
double distance(double lat1, double lon1, double lat2, double lon2) {
  /* Using Haversine Formula */

  /* convert to radians */
  double la1 = lat1*PI/180;
  double lo1 = lon1*PI/180;
  double la2 = lat2*PI/180;
  double lo2 = lon2*PI/180;

  double dla = la1-la2;
  double dlo = lo1-lo2;

  double a = pow(sin(dla/2), 2) + cos(la1) * cos(la2) * pow(sin(dlo/2), 2);

  double c = 2 * atan2(sqrt(a), sqrt(1-a));

  return c * R1;
}

double calc_local_dist() {
  int i;
  double d; 
  dist[first] = 0.0;
  incr[first] = 0.0;

  for (i = first + 1; i <= last; i++) {
    d = distance(lat[i-1], lon[i-1], lat[i], lon[i]);
    incr[i] = d;
    /* We only take the flat distance */
    dist[i] = dist[i-1] + incr[i];
  }

  return dist[last];
}

double calc_total_dist(int ptn) {
  int i;

  for (i = 1; i <= last; i++) {
      gdist[i] = gdist[i-1] + incr[i];
  }

  return gdist[ptn];
}


void define_section() {
  /* Define section of route to be handled by this process */

  bitesize = positions/procs+1;

  first = bitesize*id;
  last  = bitesize*(id+1);

  if(last >= positions) last = positions-1;

}

void print_route(){
  int i;

  printf("\n");

  printf("The route is %7.3f km long\n", global_length/1000);
}

void print_extremes(){
  printf("\n");
  printf("Max elevation: %4.0f m\n",  global_max_elev);
  printf("Min elevation: %4.0f m\n",  global_min_elev);
  printf("Max rise:      %4.0f %%\n", global_max_slope*100);
  printf("Max decline:   %4.0f %%\n", global_min_slope*100);
}

void find_extremes() {
  int i;
  int cur_pos = 0;
  double max_elev, min_elev;
  double max_slope, min_slope;

  /* Find extremes in local section */
  max_elev = min_elev = elev[first];
  max_slope = min_slope = 0.0;

  for (i = first + 1; i <= last; i++) {
    double slope = (elev[i] - elev[i-1])/incr[i];
    if (slope > max_slope)  { max_slope = slope;    }
    if (slope < min_slope)  { min_slope = slope;    }
    if (elev[i] > max_elev) { max_elev  = elev[i];  }
    if (elev[i] < min_elev) { min_elev  = elev[i];  }
  }

  /* Set global extremes */
  local_max_elev  = max_elev;
  local_min_elev  = min_elev;
  local_max_slope = max_slope;
  local_min_slope = min_slope;

}

double distance_below(double height) {
  int i;
  double total = 0.0;

  for (i = first + 1; i <= last; i++) {
    if (elev[i-1] == elev[i] && elev[i] == height) {
      /* flat strecth at exact height splits between above/below */
      total += incr[i]/2;
      continue;
    }
    if ((elev[i-1] < height && elev[i] > height) ||
        (elev[i-1] > height && elev[i] < height)   ) {
      /* strecth crossing height counts by half */
      total += incr[i]/2;
      continue;
    }
    if ((elev[i-1] <= height && elev[i] <= height) ) {
      /* strecth fully below height  */
      total += incr[i];
    }
  }

  return total;
}    

double distance_above(double height) {
  int i;
  double total = 0.0;

  for (i = first + 1; i <= last; i++) {
    if (elev[i-1] == elev[i] && elev[i] == height) {
      /* flat strecth at exact height splits between above/below */
      total += incr[i]/2;
      continue;
    }
    if ((elev[i] < height && elev[i-1] > height) ||
        (elev[i] > height && elev[i-1] < height)   ) {
      /* strecth crossing height counts by half */
      total += incr[i]/2;
      continue;
    }
    if ((elev[i] >= height && elev[i-1] >= height) ) {
      /* strecth fully above height  */
      total += incr[i];
    }
  }

  return total;
}    

void print_height_median(){
  printf("\n");
  printf("Heigth median: %3d m\n", global_height_median);
}

void print_longest_ascent(){
  printf("\n");
  printf("Longest ascent is %4.0f m\n", global_longest_ascent);
}

void find_height_median_mpi() {
  if(id == MASTER){
    int med_l = floor(global_min_elev);      // Lower bound
    int med_u =  ceil(global_max_elev);      // Upper bound

    double diff_l =  global_length;  // All route above
    double diff_u = -global_length;  // All route below                            
    int med;

    double global_diff;

    double below, above, diff;

    /* Loop invariant:  med_l <= med_u and  diff_u <= 0 <= diff_l */
    while (med_u - med_l > 1) {

    }

  }
}

void find_height_median() {
  int med_l = floor(global_min_elev);      // Lower bound
  int med_u =  ceil(global_max_elev);      // Upper bound

  double diff_l =  global_length;  // All route above
  double diff_u = -global_length;  // All route below                            
  int med;

  double global_diff;

  double below, above, diff, rdiff;

  /* Loop invariant:  med_l <= med_u and  diff_u <= 0 <= diff_l */
  while (med_u - med_l > 1) {
    /* Make a new estimate of the median */ 
    med = (med_l + med_u)/2;

    /* Find total above and below stretchtes for local section */
    if(first < last){
      above = distance_above(med);
      below = distance_below(med);
    
      diff = above - below;
    }else{
      diff = 0;
    }

    MPI_Allreduce(&diff, &rdiff, 1,
                  MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    global_diff = rdiff;

    if (global_diff == 0.0) {
      /* Hit, just record in upper bound and break */
      med_u = med;
      diff_u = global_diff;
      break;
    }

    if (global_diff > 0) {
      med_l = med;
      diff_l = global_diff;
    } else {
      med_u = med;
      diff_u = global_diff;
    }
  }

  /* Take the height with the smallest absolute difference */   
  global_height_median =  (abs(diff_u) <= abs(diff_l)) ? med_u : med_l;
}


double get_time() { 
  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);
    return (double) ( (double) time.tv_sec + (double) time.tv_nsec/1000000000);
}

int main(int argc, char* args[]){

  MPI_Init(&argc, &args);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &procs);

  if (argc < 2) {
    printf("Your must supply a route file\n");
    exit(1);
  } 
  read_route(args[1]); 

  define_section();

  //DISTANCE

  double start, end, timediff, longest;

  start = get_time();
  dist_to_send = calc_local_dist();
  end = get_time();

  timediff = end - start;

  MPI_Reduce(&timediff, &longest, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);


  MPI_Reduce(&dist_to_send, &global_length, 1, MPI_DOUBLE,
               MPI_SUM, MASTER, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Reduce(&incr, &gincr, positions, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);
  MPI_Bcast(&gincr, positions, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);

  calc_total_dist(0);

  MPI_Barrier(MPI_COMM_WORLD);

  //DISTANCE END

  //EXTREMES

  find_extremes();

  MPI_Reduce(&local_max_elev, &global_max_elev, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);
  MPI_Reduce(&local_min_elev, &global_min_elev, 1, MPI_DOUBLE, MPI_MIN, MASTER, MPI_COMM_WORLD);
  MPI_Reduce(&local_max_slope, &global_max_slope, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);
  MPI_Reduce(&local_min_slope, &global_min_slope, 1, MPI_DOUBLE, MPI_MIN, MASTER, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);

  //EXTREMES END

  if(id == MASTER){
    print_route();

    print_extremes();

    print_height_median();

    print_longest_ascent();

    printf("%f\n", longest);
  }

  MPI_Finalize();

}