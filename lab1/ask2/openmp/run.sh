#!/bin/bash

#PBS -N run
#PBS -o run.out
#PBS -e run.err
#PBS -l nodes=sandman:ppn=64
#PBS -l walltime=00:30:00

module load openmp
cd /home/parallel/parlab02/pps/lab1/ask2/openmp
for OMP_NUM_THREADS in 1 2 4 8 16 32 64
do
    export OMP_NUM_THREADS=$OMP_NUM_THREADS
    echo Number of threads: $OMP_NUM_THREADS
    ./fw_tiled 1024 32
done