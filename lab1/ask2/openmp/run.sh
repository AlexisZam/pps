#!/bin/bash

#PBS -N run-fw_tiled-4096
#PBS -o run-fw_tiled-4096
#PBS -e run-fw_tiled-4096
#PBS -l nodes=sandman:ppn=64
#PBS -l walltime=00:10:00

module load openmp
cd /home/parallel/parlab02/pps/lab1/ask2/openmp
for OMP_NUM_THREADS in 1 2 4 8 16 32 64
do
    export OMP_NUM_THREADS=$OMP_NUM_THREADS
    echo Number of threads: $OMP_NUM_THREADS
    for N in 1024 2048 4096
    do
        ./fw_tiled $N 4096
    done
done