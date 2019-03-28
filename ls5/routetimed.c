/* route.c 

02346, F19, DTU

For instructions, see Databar Exercise 4

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>


/* Route data structure */ 
#define MAX_POS 10000

double  lat[MAX_POS];
double  lon[MAX_POS];
double elev[MAX_POS];
double dist[MAX_POS];
double incr[MAX_POS];

int positions = 0;

/* Identity info (not used in this version) */
#define MASTER 0
int id, procs;

/* Local section positions */
int first, last;

/* Global properties */
double global_length;
double global_max_elev, global_min_elev;
double global_max_slope, global_min_slope;

/* Non-local properties */
double global_longest_ascent;

/* Height median */
int global_height_median;



#define PI 3.14159265358979323846
#define R1 6371009                   // Earth's  middle radius (m) 

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

void define_section() {
  /* Define section of route to be handled by this process */

  /* This version handles the full route */
  first = 0;
  last  = positions-1;
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


void calc_dist() {
  double mydist = calc_local_dist();

  global_length = mydist;
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
  global_max_elev  = max_elev;
  global_min_elev  = min_elev;
  global_max_slope = max_slope;
  global_min_slope = min_slope;
}

void find_longest_ascent() {
  int i;
  double cur_ascent = 0.0;

  global_longest_ascent = 0.0;

  for (i = 1; i < positions; i++) {
    double slope = (elev[i] - elev[i-1])/incr[i];
    if (slope > 0.0) {
      /* still going up */
      cur_ascent += incr[i];
    } else {
      /* flat or downhill again -- record ascent */
      if (cur_ascent > global_longest_ascent) {
        global_longest_ascent = cur_ascent;
      }
      /* new rise might start here! */
      cur_ascent = 0.0;
    }
  } 

  /* Do we end going up ? */
  if (cur_ascent > global_longest_ascent) {
    global_longest_ascent = cur_ascent;
  }
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

/* Find the height at which half the route is above and 
   the other half below.
   Done iteratively using bisection. 
   For quick termination, find integer height for 
   which above/below difference is smallest.
*/
void find_height_median() {
  int med_l = floor(global_min_elev);      // Lower bound
  int med_u =  ceil(global_max_elev);      // Upper bound

  double diff_l =  global_length;  // All route above
  double diff_u = -global_length;  // All route below                            
  int med;

  double global_diff;

  double below, above, diff;

  /* Loop invariant:  med_l <= med_u and  diff_u <= 0 <= diff_l */
  while (med_u - med_l > 1) {
    /* Make a new estimate of the median */ 
    med = (med_l + med_u)/2;

    /* Find total above and below stretchtes for local section */
    above = distance_above(med);
    below = distance_below(med);
    
    diff = above - below;
    
    global_diff = diff;

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

/* Printing routines */

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

void print_longest_ascent(){
  printf("\n");
  printf("Longest ascent is %4.0f m\n", global_longest_ascent);
}

void print_height_median(){
  printf("\n");
  printf("Heigth median: %3d m\n", global_height_median);
}

double get_time() { 
  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);
    return (double) ( (double) time.tv_sec + (double) time.tv_nsec/1000000000);
}
   
int main(int argc, char* args[]){

  if (argc < 2) {
    printf("Your must supply a route file\n");
    exit(1);
  } 
  read_route(args[1]);

  define_section();

  double start, end, timediff;
  start = get_time();

  calc_dist();
  end = get_time();

  timediff = end - start;
  print_route();

  printf("%f\n", timediff);
 
  find_extremes();
  print_extremes();
  
  find_longest_ascent(); 
  print_longest_ascent();

  find_height_median();
  print_height_median();


  printf("\n");
}


