#!/bin/bash
#
## run_dmm.sh -- Run DMM in GPU systems.
##
## This is an example script for submitting to the Torque system your
## experiments. You can freely change the script as you like. Please respect the
## `walltime' attribute.
##
## Please remember to compile your code with `make DEBUG=0' before
## submitting. If you plan to use this script, we recommend you to enable only
## the GPU kernels to avoid unnecessary executions of the serial and OpenMP
## version of the code wasting your time. Use similar scripts with just the
## required executions for these versions.
##
## Copyright (C) 2019, Computing Systems Laboratory (CSLab)
##

#PBS -o dmm_gpu.out
#PBS -e dmm_gpu.err
#PBS -l walltime=06:00:00
#PBS -l nodes=dungani:ppn=24:cuda

export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
export CUDA_VISIBLE_DEVICES=2

gpu_kernels="0 1 2"
problem_sizes="256 512 1024 2048"
block_sizes="$(seq 16 16 512)"
gpu_prog="./dmm_main"

## Change this to the directory of your executable!
cd where/your/executable/lies
echo "Benchmark started on $(date) in $(hostname)"
for i in $gpu_kernels; do
	for m in $problem_sizes; do
		for n in $problem_sizes; do
			for k in $problem_sizes; do
				for b in $block_sizes; do
					GPU_KERNEL=$i GPU_BLOCK_SIZE=$b $gpu_prog $m $n $k
				done
			done
		done
	done
done
echo "Benchmark ended on $(date) in $(hostname)"
