#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include <mpi.h>
#include <time.h>
#include <omp.h>

double tstart, ttotal;
long N, N_total,  sums, sums_squares, sums_recieved, sums_squares_recieved;
double stdev;
int MPI_nthreads = 1, omp_nthreads;

int *number_array; // A 1D array of char arrays (a pointer to pointers to chars)


/* myclock: (Calculates the time)
--------------------------------------------*/
double myclock() 
{
        static time_t t_start = 0;  // Save and subtract off each time

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        if( t_start == 0 ) t_start = ts.tv_sec;

        return (double) (ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9;
}


/* Creates a list of Random numbers
--------------------------------------------*/
void * Create_Number_Arrays(int myID)
{
	time_t t;
	long i;
	srand(time(NULL)+ (int) myID); // randomize seed
	// allocate an array of size N pointers to chars
	number_array = malloc(sizeof(int *)*N);

	for ( i=0; i < N; i++) 
	{
		number_array[i] = rand() % 1000; // random number from 0 to 1000
	}
	sums = 0;
	sums_squares = 0;
}


// Print the list
void print_arrays()
{
	long i;
	for ( i=0; i < N; i++) 
	{
		printf("%d\n",number_array[i] ); // random number from 0 to 1 million
	}
} 


/* Calculare the Sum of the list
--------------------------------------------*/
void * Calculate_Sums(void *myID)
{
	long i;
	long local_sums;
	long local_sums_squares;
	int startPos, endPos;

	#pragma omp private(myID,startPos,endPos,local_sums,local_sums_squares,i)
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

		//printf("%d-%ld-%d-%d\n",omp_get_thread_num(),local_sums,startPos,endPos);

		// sum up the partial sum into the global sum
		#pragma omp critical
		sums += local_sums;
		sums_squares += local_sums_squares;
	}
}

/* Calculate the Standard Deviation
--------------------------------------------*/
void * Calculate_Standard_Deviation()
{
	stdev =  sqrt( ((double)sums_squares - ((double)sums*sums)/ N )/ (N-1) );
}

/*  Main
--------------------------------------------*/
int main(int argc, char *argv[]) 
{
	
	struct rusage ru;
	// Default Value
	N_total   = 10;	
	omp_nthreads = 1;	
	if (argc >= 2){
		omp_nthreads = atol(argv[1]);
		N_total = atol(argv[2]);
	}

	// Initalize the Open Mp Enviroment
	omp_set_num_threads(omp_nthreads);

	// Initalize the MPI Enviroment
	int i, rc, rank;
	
	MPI_Status Status;
    MPI_Request Request;
	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS){
		printf ("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
    MPI_Comm_size(MPI_COMM_WORLD,&MPI_nthreads); // Number of cores
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);	 // rank of each core


    if (rank ==0)
    {
		printf("Start MPI/OpenMP Process:\n");
    	tstart = myclock(); // Global Clock
	}
    // Create Number Arrays
    // -------------------------

	MPI_Bcast(&N_total, 1, MPI_LONG, 0, MPI_COMM_WORLD);

	N = N_total / MPI_nthreads;

	Create_Number_Arrays(rank); // All mpi process will create a random list
	//print_arrays();

	// Calculate Sums
	#pragma omp parallel
	{
		Calculate_Sums(omp_get_thread_num());
	}

	getrusage(RUSAGE_SELF, &ru);
    long MEMORY_USAGE = ru.ru_maxrss;   // Memory usage in Kb


	if (rank != 0)
	{
		MPI_Isend(&sums, 1, MPI_LONG, 0, 1234, MPI_COMM_WORLD, &Request); 
		MPI_Isend(&sums_squares, 1, MPI_LONG, 0, 5678, MPI_COMM_WORLD, &Request); 
		printf("Done rank %d\n",rank);
	}
	else
	{

		for (i = 1; i< MPI_nthreads; i++)
		{
			MPI_Irecv(&sums_recieved, 1, MPI_LONG, i, 1234, MPI_COMM_WORLD, &Request);
			sums += sums_recieved ;		

			MPI_Irecv(&sums_squares_recieved, 1, MPI_LONG, i, 5678, MPI_COMM_WORLD, &Request);
			sums_squares += sums_squares_recieved ;		
		}

		Calculate_Standard_Deviation();
		ttotal = myclock() - tstart;
		printf("Part3-MPI-%d/OpenMP-%d:%ld,%lf,%lf,%ld\n",MPI_nthreads,omp_nthreads,N_total,stdev,ttotal,MEMORY_USAGE);
	} 

	MPI_Finalize();
	return 0;
}
