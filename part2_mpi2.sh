#!/bin/bash
#$ -l mem=5G
#$ -l h_rt=24:00:00
#$ -l killable
#$ -cwd
#$ -P KSU-GEN-RESERVED
#$ -q \*@@elves
#$ -pe mpi-8 16

echo "PE_HOSTFILE:"
echo $PE_HOSTFILE
echo
echo "cat PE_HOSTFILE:"
cat $PE_HOSTFILE
echo '-----'
sed 's/ 8/ slots=1/g' $PE_HOSTFILE > NEW_HOSTFILE0
sed -i 's/gen-reserved.q@elf...beocat.ksu.edu UNDEFINED//g' NEW_HOSTFILE0

# OpenMp = 2
for i in 100000000 200000000 300000000 400000000 500000000 600000000 700000000
do
mpirun --hostfile NEW_HOSTFILE0 -np 2  /homes/mcheikh/CIS_625/hw5/part-2/Part2-MPI-OpenMP 8 0 100 $i 
mpirun --hostfile NEW_HOSTFILE0 -np 2  /homes/mcheikh/CIS_625/hw5/part-2/Part2-MPI-OpenMP 4 0 100 $i 
mpirun --hostfile NEW_HOSTFILE0 -np 2  /homes/mcheikh/CIS_625/hw5/part-2/Part2-MPI-OpenMP 2 0 100 $i 

echo -e "-----------DONE-----------\n"
done
