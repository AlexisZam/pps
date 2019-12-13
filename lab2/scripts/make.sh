#!/bin/bash

#PBS -N make
#PBS -o make.out
#PBS -e make.err
#PBS -l nodes=1:ppn=1
#PBS -l walltime=00:10:00

module load openmpi/1.8.3
cd /home/parallel/parlab02/pps/lab2/mpi
make
