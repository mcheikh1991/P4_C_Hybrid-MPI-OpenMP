#!/bin/bash
#$ -l mem=5G
#$ -l h_rt=24:00:00
#$ -l killable
#$ -cwd
#$ -P KSU-GEN-RESERVED
#$ -q \*@@elves
#$ -pe mpi-4 16

echo "PE_HOSTFILE:"
echo $PE_HOSTFILE
echo
echo "cat PE_HOSTFILE:"
cat $PE_HOSTFILE
echo '-----'

sed 's/ 4/ slots=1/g' $PE_HOSTFILE > NEW_HOSTFILE3
sed -i 's/gen-reserved.q@elf...beocat.ksu.edu UNDEFINED//g' NEW_HOSTFILE3

# OpenMp = 2
for i in 2 3 4
do
mpirun --hostfile NEW_HOSTFILE3 -np 4  /homes/mcheikh/CIS_625/hw5/part-1/Part1-MPI-OpenMP 4 1 1 $i $i
mpirun --hostfile NEW_HOSTFILE3 -np 1  /homes/mcheikh/CIS_625/hw5/part-1/Part1-MPI-OpenMP 4 1 1 $i $i

echo -e "-----------DONE-----------\n"
done
