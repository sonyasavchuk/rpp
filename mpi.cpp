#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctime>
#include <random>
#include <cstring>
#include <algorithm>
#include <mpi.h>


using namespace std;



int procNum;
int procRank;


double targetFunction(double x) {
	return x*x;
}


void processInitialization(int &n, double &l, double &r,  
	double* &results, double &procX0, int &procK, double &h) {
	if (procRank == 0) {
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
		} while(n <= 0);
		h = (r - l) / (double)n;
	}
	
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&h, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&l, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	int restSegments = n;
	for (int i = 0; i < procRank; i++) { 
		restSegments = restSegments-restSegments/(procNum-i);
	}
	procX0 = l + h*(n-restSegments);
	procK = restSegments/(procNum-procRank);
	
	results = new double[procNum];
	
	return;
}


void serialResultCalculation(double x0, int k, double h, 
	double &result) {
	
	result = 0;
	double prevX = x0;
	for (int i = 1; i <= k; i++) {
		double nextX = prevX + h;
		result += (targetFunction(prevX) + 
			4*targetFunction(0.5*(prevX+nextX)) + 
			targetFunction(nextX)) * (nextX-prevX) / 6.0;
		prevX = nextX;
	}
	
	return;
}




void resultReplication(double &procResult, double* results) {
	MPI_Allgather(&procResult, 1, MPI_DOUBLE,
		results, 1, MPI_DOUBLE, MPI_COMM_WORLD);
	return;
}




void sumSegmentsResults(double *results, double &result) {
	result = 0;
	for (int i = 0; i < procNum; i++) {
		result += results[i];
	}
	return;
}


int main(int argc, char **argv) {
	setbuf(stdout, NULL);
	
	int n;
	double l, r, h;
	
	double result;
	
	
	
	double procX0;
	int procK;
	double procResult;
	double *results;
	double start, finish, duration;
	
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &procNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

	if (procRank == 0) {
		printf ("Parallel program\n");
	}
	
	processInitialization(n, l, r, results, procX0, procK, h);
	
	//printf("proc=%d x0=%lf k=%d\n",procRank, procX0, procK);
	
	start = MPI_Wtime();

	serialResultCalculation(procX0, procK, h, procResult);	
	resultReplication(procResult, results);

	sumSegmentsResults(results, result);
		
	finish = MPI_Wtime();
	duration = finish - start;
	
	
	if (procRank == 0) {
		printf("Time of execution = %f\n", duration);
		printf("Result            = %f\n", result);
	}
	
	
	MPI_Finalize();
	
	return 0;
}
