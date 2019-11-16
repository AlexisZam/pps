#!/bin/bash

#PBS -N run-fw_sr-4096
#PBS -o run-fw_sr-4096.out
#PBS -e run-fw_sr-4096.err
#PBS -l nodes=sandman:ppn=64
#PBS -l walltime=00:10:00

module load openmp
cd /home/parallel/parlab02/pps/lab1/ask2/openmp
export OMP_DISPLAY_ENV=true
export OMP_SCHEDULE=static
export OMP_PROC_BIND=true
for OMP_NUM_THREADS in 1 2 4 8 16 32 64
do
    export OMP_NUM_THREADS=$OMP_NUM_THREADS
    echo Number of threads: $OMP_NUM_THREADS
    for N in 1024 2048 4096
    do
        ./fw_sr $N 4096
    done
done