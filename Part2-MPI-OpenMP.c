#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include <mpi.h>
#include <time.h>
#include <omp.h>

#define PI 3.14159265

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

double tstart, ttotal;
int MPI_nthreads = 1, omp_nthreads;
double x_0,x_end;
long N_Total, N; // Number of points
double min_f;

double *number_array;
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


/* Creates a list of Random numbers from x_0 to x_end
--------------------------------------------------------*/
void * Create_Number_Arrays(long n, int myID)
{
	long i;
	srand(time(NULL)+ (int) myID); // randomize seed
	int Z = 0;
	// allocate an array of size n pointers to chars
	number_array = malloc(sizeof(double *)*n);
	for ( i=0; i < n; i++) 
	{
		Z = rand()% 1000000;
		number_array[i] = x_0 + (Z/1000000.0)*(x_end - x_0); // random number from x_0 to X_end 
	}
}


/* Function that we wish to optimize
--------------------------------------------*/
double f (double x)
{
	return cos(x)+(pow(fabs(7.0-x), 2.0/15.0))+2*(pow(fabs(5.0-x), 4.0/35.0));
}


/* Monte Carlo Method 
-----------------------*/

void * Monte_Carlo_Method (void* rank_OpenMP)
{

	min_f = f(x_0);
	double local_min_f; 
	
	// OpenMP ID's
	int myOp_ID;

	// Start j and End j controlled by OpenMp
	int i, i_start, i_end;
	double x;

	#pragma omp private(myOp_ID,i_start,i_end,i,local_min_f,x)
    {

		myOp_ID =  ((int) rank_OpenMP);
    	local_min_f = f(x_0);

		// Start i and End i controlled by MPI
	 	i_start = ( myOp_ID) * (N / omp_nthreads);
	 	i_end = i_start + (N / omp_nthreads);

        // Initialize Local Integration
		for(i = i_start; i < i_end; i++)
		{
			x = number_array[i];
			local_min_f = MIN(f(x),local_min_f);
		}

		#pragma omp critical
		{
			if (local_min_f < min_f) 
            min_f = local_min_f;
		}
	}

}

/*  Main
--------------------------------------------*/
int main(int argc, char *argv[]) 
{
	struct rusage ru;
	// Default Value
	omp_nthreads = 1;	
	x_0 ,x_end = 0.0,10.0;
	N_Total = 10;
	if (argc >= 2){
		omp_nthreads = atol(argv[1]);
		sscanf(argv[2],"%lf",&x_0);
		sscanf(argv[3],"%lf",&x_end);
		sscanf(argv[4],"%ld",&N_Total); // Number of points
	}

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

	// Initalize the Open Mp Enviroment
	omp_set_num_threads(omp_nthreads);

    N = N_Total / MPI_nthreads; // number of points for each MPI Process

    if (rank ==0)
    {
		printf("Start MPI/OpenMP Process:\n");
    	tstart = myclock(); // Global Clock
	}
	
	// Create the array of random numbers from x_0 to x_end
	Create_Number_Arrays(N,rank);

	// Find the Min using monte carlo for each MPI Process 
	
	#pragma omp parallel
	{
		Monte_Carlo_Method ( omp_get_thread_num() );
	}

	getrusage(RUSAGE_SELF, &ru);
    long MEMORY_USAGE = ru.ru_maxrss;   // Memory usage in Kb

    // Send the data to MPI
	if (rank != 0)
	{
		MPI_Isend(&min_f , 1, MPI_DOUBLE, 0, 1234, MPI_COMM_WORLD, &Request); 
		printf("Done rank %d\n",rank);
	}
	
	MPI_Barrier( MPI_COMM_WORLD ) ; 

	if (rank == 0)
	{
		double local_min_f=0.0;

		for (i = 1; i< MPI_nthreads; i++)
		{
			MPI_Irecv(&local_min_f, 1, MPI_DOUBLE, i, 1234, MPI_COMM_WORLD, &Request);
			min_f = MIN(local_min_f,min_f);
		}

		ttotal = myclock() - tstart;
		printf("Part2-MPI-%d/OpenMP-%d:%ld,%lf,%lf,%ld\n",MPI_nthreads,omp_nthreads,N_Total,min_f,ttotal,MEMORY_USAGE);
	} 

	MPI_Finalize();
	return 0;
}
