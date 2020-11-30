#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

long N,  sums, sums_squares, sums_recieved, sums_squares_recieved;
double stdev;
int nthreads = 1, omp_nthreads;

int *number_array; // A 1D array of char arrays (a pointer to pointers to chars)

void * Create_Number_Arrays()
{
	time_t t;
	long i;
	srand(time(NULL)); // randomize seed
	// allocate an array of size N pointers to chars
	number_array = malloc(sizeof(int *)*N);

	for ( i=0; i < N; i++) 
	{
		number_array[i] = rand() % 1000; // random number from 0 to 1 million
	}
	sums = 0;
	sums_squares = 0;
}


void * Calculate_Sums(void *myID)
{
	long i;
	long local_sums;
	long local_sums_squares;
	int startPos, endPos;

	#pragma omp private(myID,startPos,endPos,local_sums,local_sums_squares,endPos,i)
    {
    	startPos = ((int) myID) * (N / omp_nthreads);
        endPos = startPos + (N / omp_nthreads);

        // Initialize Local sums
        local_sums = 0;
        local_sums_squares = 0;

		for ( i= startPos; i < endPos; i++) 
		{
			local_sums+= number_array[i] ; // random number from 0 to 1 million
			local_sums_squares += number_array[i]*number_array[i];
		}

		// sum up the partial sum into the global sum
		#pragma omp critical
		sums += local_sums;
		sums_squares += local_sums_squares;
	}
}

void Calculate_Standard_Deviation()
{
	stdev =  sqrt( ((double)sums_squares - ((double)sums*sums)/ N )/ (N-1) );
}


void print_arrays()
{
	long i;
	for ( i=0; i < N; i++) 
	{
		printf("%d\n",number_array[i] ); // random number from 0 to 1 million
	}
} 

int main (int argc, char *argv[]) 
{
	// Default Value
	N   = 10;	
	omp_nthreads = 1;	
	if (argc >= 2){
		N = atol(argv[1]);
		omp_nthreads = atol(argv[2]);
	}

	int rank;

	// Initalize the Open Mp Enviroment
	omp_set_num_threads(omp_nthreads);

/* Fork a team of threads giving them their own copies of variables */
	// Create Number Arrays

	printf("Start OpenMP Process:\n");

	Create_Number_Arrays();

	// Calculate Sums
	#pragma omp parallel
	{
		Calculate_Sums(omp_get_thread_num());
	}

	//print_arrays();

	Calculate_Standard_Deviation();

	printf("OpenMP-%d:%ld,%ld,%lf\n",omp_nthreads,sums,sums_squares,stdev );

}

