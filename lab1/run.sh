#!/bin/bash

## Give the Job a descriptive name
#PBS -N runjob

#PBS -o runjob.out
#PBS -e runjob.err

## How many machines should we get? 
#PBS -l nodes=1:ppn=8

##How long should the job run for?
#PBS -l walltime=00:10:00

## Start 
## Run make in the src folder (modify properly)
module load openmp
cd /home/parallel/parlab02/pps/a1/code
for OMP_NUM_THREADS in 1 2 4 6 8
do
    export OMP_NUM_THREADS=$OMP_NUM_THREADS
    echo $OMP_NUM_THREADS 
    for N in 64 1024 4096
    do
        ./Game_Of_Life $N 1000
    done
done
