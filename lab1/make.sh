#!/bin/bash

## Give the Job a descriptive name
#PBS -N makejob

## Output and error files
#PBS -o makejob.out
#PBS -e makejob.err

## How many machines should we get?
#PBS -l nodes=1

## Start 
## Run make in the src folder (modify properly)
cd /home/parallel/parlab02/pps/a1/code
make

