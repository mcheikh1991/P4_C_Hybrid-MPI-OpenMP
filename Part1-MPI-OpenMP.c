#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include <mpi.h>
#include <time.h>
#include <omp.h>

#define PI 3.14159265

double tstart, ttotal;
double  volume ,global_volume;
int MPI_nthreads = 1, omp_nthreads;
double x,y,x_end,y_end;
char processor_name[20];

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


/* Easom’s Function 
--------------------------------------------*/
double f (double x, double y)
{
	return -cos(x)*sin(y)*exp( -((x - PI)*(x - PI) + (y - PI)*(y - PI)) );
}


/* Calculare the Integral of the list
--------------------------------------------*/

void * Integrate (void *rank_MPI, void* rank_OpenMP)
{

	double local_volume;
	double h = 0.00001;
	
	// MPI and OpenMP ID's
	int myMPI_ID =  *((int*) rank_MPI);
	int myOp_ID;

	// Total number of points
	long N_i = (x_end - x) / h; 
	long N_j = (y_end - y) / h;

	// Start i and End i controlled by MPI
	int i_start = (myMPI_ID) * (N_i / MPI_nthreads);
	int i_end = i_start + (N_i / MPI_nthreads);

	// Start j and End j controlled by OpenMp
	int j_start, j_end;

	int i,j;
	
	#pragma omp private(rank_OpenMP,j_start,j_end,local_volume,i,j)
    {

    	printf("Thread %d out of %d from process %d out of %d on %s\n",
        omp_get_thread_num(), omp_get_num_threads(), myMPI_ID, MPI_nthreads, processor_name);
		
		myOp_ID =  ((int) rank_OpenMP);
		
		local_volume = 0.0;

		// Start i and End i controlled by MPI
	 	j_start = ( myOp_ID) * (N_j / omp_nthreads);
	 	j_end = j_start + (N_j / omp_nthreads);

        // Initialize Local Integration
		for(i = i_start; i < i_end; i++)
		{
			for(j = j_start; j < j_end; j++)
				{
				local_volume += h*h*f(x + i*h + h/2.0, y + j*h + h/2.0);
				}
		}

		#pragma omp critical
		volume += local_volume;
	}

}

/*  Main
--------------------------------------------*/
int main(int argc, char *argv[]) 
{
	volume ,global_volume = 0.0, 0.0;
	struct rusage ru;
	// Default Value
	omp_nthreads = 1;	
	x , y , x_end ,y_end = 1.0 ,1.0,5.0,5.0;
	if (argc >= 2){
		omp_nthreads = atol(argv[1]);
		sscanf(argv[2],"%lf",&x);
		sscanf(argv[3],"%lf",&y);
		sscanf(argv[4],"%lf",&x_end);
		sscanf(argv[5],"%lf",&y_end);
	}

	// Initalize the MPI Enviroment
	int i, rc, rank, namelen;
	
	MPI_Status Status;
    MPI_Request Request;
	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS){
		printf ("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
    MPI_Comm_size(MPI_COMM_WORLD,&MPI_nthreads); // Number of cores
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);	 // rank of each core
	MPI_Get_processor_name(processor_name, &namelen); // Processors name

	// Initalize the Open Mp Enviroment
	omp_set_num_threads(omp_nthreads);

	printf("rank:%d,x0:%lf,x_end:%lf,y0:%lf,y_end:%lf\n",rank,x,x_end,y,y_end);

    if (rank ==0)
    {
		printf("Start MPI/OpenMP Process:\n");
    	tstart = myclock(); // Global Clock
	}

	// Calculate Integral
	#pragma omp parallel
	{
		Integrate ( &rank, omp_get_thread_num() );
	}


	getrusage(RUSAGE_SELF, &ru);
    long MEMORY_USAGE = ru.ru_maxrss;   // Memory usage in Kb

	MPI_Reduce(&volume, &global_volume, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if(rank==0)
	{
		ttotal = myclock() - tstart;
		long N_j = (y_end - y) / 0.00001;
		printf("Part1-MPI-%d/OpenMP-%d:%ld,%lf,%lf,%lf,%lf,%.12lf,%lf,%ld\n",MPI_nthreads,omp_nthreads,N_j,x,x_end,y,y_end,global_volume,ttotal,MEMORY_USAGE);
		printf("------------\n");
	}
	MPI_Finalize();

	return 0;
}
