# Project 4: C-Language Hybrid MPI-OpenMP
The current repository presents three codes each solving one of the three problems defined below. The purpose behind the codes is to combine the functionality of OpenMP with MPI the solve each problem. The problems are defined below:

1. The first problem requires us to write a code that performs numerical integration on Easom’s function on a grid that is 1/100,000,000. Easom's function is,
f(x, y) = -cos(x)*ssin(y)*exp( -((x - PI)*(x - PI) + (y - PI)*(y - PI)) )

2. The second problem requires us to write a code that tests the performance of several numerical optimization methods on a particular function
f(x) = cos(x) + (pow(fabs(7.0-x)), 2.0/15.0)) + 2(pow(fabs(5.0-x)), 4.0/35.0)
The program will use a Monte-Carlo method to analyze the function (i.e generate a whole bunch of points within the range given at the command line and evaluate them to find the minimum). 

3. The third problem requires us to write an MPI program that will compute the standard deviation of a set of randomly generated numbers. The code should Assume that processes will generate these numbers independently and send their results (sums and sums of squares) to process rank 0 for computation of the standard deviation.

The results are show in the "Report.pdf" file.


