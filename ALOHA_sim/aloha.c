#include <stdio.h>              // Needed for printf()
#include <stdlib.h>             // Needed for exit() and rand()
#include <math.h>               // Needed for log()

#define SIM_TIME   1.0e6        // Simulation time
#define ARR_TIME   1.25         // Mean time between arrivals
#define SERV_TIME  1.00         // Mean service time

double expntl(double x);        // Generate exponential RV with mean x

int main(void){
  double   end_time = SIM_TIME; // Total time to simulate
  double   Ta = ARR_TIME;       // Mean time between arrivals
  double   Ts = SERV_TIME;      // Mean service time

  double   time = 0.0;          // Simulation time
  double   t1 = 0.0;            // Time for next event #1 (arrival)
  double   t2 = SIM_TIME;       // Time for next event #2 (departure)
  unsigned int n = 0;           // Number of customers in the system

  unsigned int c = 0;           // Number of service completions
  double   b = 0.0;             // Total busy time
  double   s = 0.0;             // Area of number of customers in system
  double   tn = time;           // Variable for "last event time"
  double   tb;                  // Variable for "last start of busy time"
  double   x;                   // Throughput
  double   u;                   // Utilization
  double   l;                   // Mean number in the system
  double   w;                   // Mean residence time

  // Main simulation loop
  while (time < end_time){
    if (t1 < t2){                // *** Event #1 (arrival) ***
      time = t1;
      s = s + n * (time - tn);  // Update area under "s" curve
      n++;
      tn = time;                // tn = "last event time" for next event
      t1 = time + expntl(Ta);
      if (n == 1){
        tb = time;              // Set "last start of busy time"
        t2 = time + expntl(Ts);
      }
    }else{                        // *** Event #2 (departure) ***
      time = t2;
      s = s + n * (time - tn);  // Update area under "s" curve
      n--;
      tn = time;                // tn = "last event time" for next event
      c++;                      // Increment number of completions
      if (n > 0)
        t2 = time + expntl(Ts);
      else{
        t2 = SIM_TIME;
        b = b + time - tb;      // Update busy time sum if empty
      }
    }
  }

  x = c / time;   // Compute throughput rate
  u = b / time;   // Compute server utilization
  l = s / time;   // Compute mean number in system
  w = l / x;      // Compute mean residence or system time

  printf("--Results from simulation--\n");
  printf("Total simulated time : %3.4f s\n", end_time);
  printf("INPUTS:\n");
  printf("Mean time between arrivals : %f s\n", Ta);
  printf("Mean service time : %f s\n", Ts);
  printf("OUTPUTS:\n");
  printf("Number of completions : %d cust    \n", c);
  printf("Throughput rate : %f cust/sec \n", x);
  printf("Server utilization : %f %%       \n", 100.0 * u);
  printf("Mean number in system : %f cust     \n", l);
  printf("Mean residence time : %f s\n", w);
}

//Function to generate exponentially distributed RVs using inverse method
//Input:  x (mean value of distribution)                                
//Output: Returns with exponential RV                                   

double expntl(double x)
{
  double z;                     // Uniform random number from 0 to 1

  // Pull a uniform RV (0 < z < 1)
  do
  {
    z = ((double) rand() / RAND_MAX);
  }
  while ((z == 0) || (z == 1));

  return(-x * log(z));
}
