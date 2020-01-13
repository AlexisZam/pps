#!/bin/bash

#PBS -N run
#PBS -o run.out
#PBS -e run.err
#PBS -l nodes=sandman:ppn=64
#PBS -l walltime=00:10:00

cd /home/parallel/parlab02/pps/lab3/z3
for nthreads in 1 2 4 8 16 32 64
do
    for list_size in 1024 8192
    do
        for pcts in "80 10 10" "20 40 40"
        do
            echo List Size: $list_size
            export MT_CONF=$(seq -s , 0 $(($nthreads - 1)))
            ./linked_list $list_size $pcts
        done
    done
done
