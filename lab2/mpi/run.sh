#!/bin/bash

#PBS -N run
#PBS -o run.out
#PBS -e run.err
#PBS -l nodes=2:ppn=8
#PBS -l walltime=00:10:00

module load openmpi/1.8.3
cd /home/parallel/parlab02/pps/lab2/mpi
mpirun -np 16 ./jacobi
