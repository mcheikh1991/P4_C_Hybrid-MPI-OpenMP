#!/bin/bash

#mpicc -o Part3-MPI Part3-MPI.c -lm

#mpirun -np 4 Part3-MPI

#echo '------------------------------'

#gcc -fopenmp -o Part3-OpenMP Part3-OpenMP.c -lm

#./Part3-OpenMP 10 3


mpicc -O2 -fopenmp -o Part3-MPI-OpenMP Part3-MPI-OpenMP.c -lm 

echo '------------------------------'

mpirun -np 1 Part3-MPI-OpenMP 1 1000
mpirun -np 1 Part3-MPI-OpenMP 2 1000
mpirun -np 1 Part3-MPI-OpenMP 4 1000
#mpirun -np 2 Part3-MPI-OpenMP 1 1000
#mpirun -np 2 Part3-MPI-OpenMP 2 1000
echo '------------------------------'