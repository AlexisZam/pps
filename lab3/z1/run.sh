#!/bin/bash

#PBS -N run
#PBS -o run.out
#PBS -e run.err
#PBS -l nodes=sandman:ppn=64
#PBS -l walltime=00:30:00

cd /home/parallel/parlab02/pps/lab3/z1
i=1
for MT_CONFS in "0-0 0-1 0-3 0-7 0-7,32-39 0-15,32-47 0-63" "0-0 0-0,8-8 0-0,8-8,16-16,24-24 0-1,8-9,16-17,24-25 0-3,8-11,16-19,24-27 0-31 0-63"; do
    echo Run: $i
    for MT_CONF in $MT_CONFS; do
        x=""
        for y in ${MT_CONF//,/ }; do
            x+=$(seq -s , ${y/-/ }),
        done
        export MT_CONF=${x::-1}
        ./accounts
    done
    i=$(($i + 1))
done
