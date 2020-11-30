#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include <mpi.h>
#include <time.h>

long N,  sums, sums_squares, sums_recieved, sums_squares_recieved;
double stdev;
int nthreads = 1;

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
		number_array[i] = rand() % 10000; // random number from 0 to 1 million
	}
	sums = 0;
	sums_squares = 0;
}


void print_arrays()
{
	long i;
	for ( i=0; i < N; i++) 
	{
		printf("%d\n",number_array[i] ); // random number from 0 to 1 million
	}
} 


void Calculate_Sums()
{
	long i;

	for ( i=0; i < N; i++) 
	{
		sums+= number_array[i] ; // random number from 0 to 1 million
		sums_squares += number_array[i]*number_array[i];
	}
}

void Calculate_Standard_Deviation()
{
	stdev =  sqrt( ((double)sums_squares - ((double)sums*sums)/ N )/ (N-1) );
}

int main(int argc, char *argv[]) 
{

	N   = 10;	// Default Value	
	if (argc >= 2){
		N = atol(argv[1]);
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
    MPI_Comm_size(MPI_COMM_WORLD,&nthreads); // Number of cores
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);	 // rank of each core

    if(rank==0)  	
	printf("Start MPI Process:\n");

    // Create Number Arrays
	Create_Number_Arrays(&rank);

	// Calculate Sums
	Calculate_Sums();

	if (rank != 0)
	{
		MPI_Isend(&sums, 1, MPI_LONG, 0, 1234, MPI_COMM_WORLD, &Request); 
		MPI_Isend(&sums_squares, 1, MPI_LONG, 0, 5678, MPI_COMM_WORLD, &Request); 
	}
	else
	{
		for (i = 1; i< nthreads; i++)
		{
			MPI_Irecv(&sums_recieved, 1, MPI_LONG, i, 1234, MPI_COMM_WORLD, &Request);
			sums += sums_recieved ;		

			MPI_Irecv(&sums_squares_recieved, 1, MPI_LONG, i, 5678, MPI_COMM_WORLD, &Request);
			sums_squares += sums_squares_recieved ;		
		}

		Calculate_Standard_Deviation();

		printf("MPI-%d:%ld,%ld,%lf\n",nthreads,sums,sums_squares,stdev );
	}

	//print_arrays();

	printf("Done rank %d\n",rank);

	MPI_Finalize();
	return 0;
}
