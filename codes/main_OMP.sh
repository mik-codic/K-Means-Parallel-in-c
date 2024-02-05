#!/bin/bash
#PBS -l select=1:ncpus=1:mem=2gb -l place=pack

#PBS -l walltime=0:02:00

#PBS -q short_cpuQ
gcc -g -Wall -fopenmp -o OMP_parallel OMP_parallel.c -std=c99 -lm
./OMP_parallel 4