#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctime>
#include <random>
#include <cstring>
#include <algorithm>
#include <omp.h>

using namespace std;

int n;
double l, r, h;
double result;

double targetFunction(double x) {
  return x * x;
}

void processInitialization(int& n, double& l, double& r, double& h) {
  if (omp_get_thread_num() == 0) {
    do {
      printf("Input integration segment\n");
      printf("l:");
      scanf("%lf", &l);
      printf("r:");
      scanf("%lf", &r);
      printf("Chosen segment: [%f, %f]\n", l, r);
      if (l > r) {
        printf("r must be greater than l!\n");
      }
    } while (l > r);

    do {
      printf("Input number of split segments:");
      scanf("%d", &n);
      printf("Chosen number of segments: %d\n", n);
      if (n <= 0) {
        printf("n must be greater than 0!\n");
      }
    } while (n <= 0);
    h = (r - l) / (double)n;
  }

  
}

void serialResultCalculation(double x0, int k, double h, double& result) {
  result = 0;
  double prevX = x0;
  #pragma omp parallel for reduction(+: result)
  for (int i = 1; i <= k; i++) {
    double nextX = prevX + h;
    result += (targetFunction(prevX) + 4 * targetFunction(0.5 * (prevX + nextX)) +
               targetFunction(nextX)) *
              (nextX - prevX) / 6.0;
    prevX = nextX;
  }
}

int main() {
  setbuf(stdout, NULL);

  processInitialization(n, l, r, h);
  int numThreads = 8; 
  omp_set_num_threads(numThreads);

  double start = omp_get_wtime();

  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    int numThreads = omp_get_num_threads();

    int procK = n / numThreads;
    int remainder = n % numThreads;

    if (tid < remainder) {
      procK++;
    }

    double procX0 = l + tid * h * procK;
    double procResult = 0.0;

    serialResultCalculation(procX0, procK, h, procResult);

    #pragma omp critical
    result += procResult;
  }

  double finish = omp_get_wtime();
  double duration = finish - start;

  printf("Time of execution = %f\n", duration);
  printf("Result            = %f\n", result);

  return 0;
}
