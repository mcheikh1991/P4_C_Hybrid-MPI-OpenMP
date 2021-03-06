#!/bin/bash
#$ -l mem=5G
#$ -l h_rt=24:00:00
#$ -l killable
#$ -cwd
#$ -P KSU-GEN-RESERVED
#$ -q \*@@elves
#$ -pe single 8

# MPI = 1
for i in 100000000 200000000 300000000 400000000 500000000 600000000 700000000
do
mpirun -np 1  /homes/mcheikh/CIS_625/hw5/part-3/Part3-MPI-OpenMP 8 $i 
mpirun -np 1  /homes/mcheikh/CIS_625/hw5/part-3/Part3-MPI-OpenMP 1 $i 

echo -e "-----------DONE-----------\n"
done
