#!/bin/bash
#$ -l mem=5G
#$ -l h_rt=24:00:00
#$ -l killable
#$ -cwd
#$ -P KSU-GEN-RESERVED
#$ -q \*@@elves
#$ -pe mpi-spread 16

# Openmp 1
for i in 2 3 4
do
mpirun -np 16  /homes/mcheikh/CIS_625/hw5/part-1/Part1-MPI-OpenMP 1 1 1 $i $i
mpirun -np 8  /homes/mcheikh/CIS_625/hw5/part-1/Part1-MPI-OpenMP 1 1 1 $i $i
mpirun -np 4  /homes/mcheikh/CIS_625/hw5/part-1/Part1-MPI-OpenMP 1 1 1 $i $i
mpirun -np 2  /homes/mcheikh/CIS_625/hw5/part-1/Part1-MPI-OpenMP 1 1 1 $i $i

echo -e "-----------DONE-----------\n"
done
