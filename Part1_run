#!/bin/bash

mpicc -O2 -fopenmp -o Part1-MPI-OpenMP Part1-MPI-OpenMP.c -lm 

echo '------------------------------'

mpirun -np 1 Part1-MPI-OpenMP 1 1 1 1.01 1.01
mpirun -np 2 Part1-MPI-OpenMP 1 1 1 1.01 1.01
mpirun -np 1 Part1-MPI-OpenMP 2 1 1 1.01 1.01
mpirun -np 2 Part1-MPI-OpenMP 2 1 1 1.01 1.01
