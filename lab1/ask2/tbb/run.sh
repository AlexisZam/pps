#!/bin/bash

#PBS -N run
#PBS -o run.out
#PBS -e run.err
#PBS -l nodes=sandman:ppn=64
#PBS -l walltime=00:10:00

module load tbbz
cd /home/parallel/parlab02/pps/lab1/ask2/tbb
for nthreads in 1 2 4 8 16 32 64
do
    echo Number of threads: $nthreads
    for N in 1024 2048 4096
    do
        ./fw_tiled $N 64 $nthreads
    done
done