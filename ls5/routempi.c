/* routempi.c 
  parralel implementation of route.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define GENERIC_MAX(x, y) ((x) > (y) ? (x) : (y))
#define GENERIC_MIN(x, y) ((x) < (y) ? (x) : (y))

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
double dist_to_send;
double recvbuf;

double global_length;
double global_max_elev, global_min_elev;
double global_max_slope, global_min_slope;

double local_max_elev, local_min_elev;
double local_max_slope, local_min_slope;

/* Non-local properties */
double global_longest_ascent;

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


void define_section() {
  /* Define section of route to be handled by this process */

  int bitesize;
  bitesize = positions/procs+1;

  first = bitesize*id;
  last  = bitesize*(id+1);

  if(last >= positions) last = positions-1;

  //printf("Using section %d:%d\n", first,last);

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

  dist_to_send = calc_local_dist();

  MPI_Reduce(&dist_to_send, &global_length, 1, MPI_DOUBLE,
               MPI_SUM, MASTER, MPI_COMM_WORLD);

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
  }

  MPI_Finalize();

}