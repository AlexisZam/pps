#!/bin/bash

#PBS -N run
#PBS -o run.out
#PBS -e run.err
#PBS -l nodes=1:ppn=8
#PBS -l walltime=00:10:00

module load openmp
cd /home/parallel/parlab02/pps/lab1/ask1
for OMP_NUM_THREADS in 1 2 4 6 8
do
    export OMP_NUM_THREADS=$OMP_NUM_THREADS
    echo Number of threads: $OMP_NUM_THREADS
    for N in 64 1024 4096
    do
        ./Game_Of_Life $N 1000
    done
done
