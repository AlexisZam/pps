#!/bin/bash

#PBS -N makejob

#PBS -o makejob.out
#PBS -e makejob.err

#PBS -l nodes=1

cd /home/parallel/parlab02/pps/lab1/ask1
make

