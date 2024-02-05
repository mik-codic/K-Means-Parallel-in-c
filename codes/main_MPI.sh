#!/bin/bash
#PBS -l select=1:ncpus=2:mem=2gb
#PBS -l walltime=0:05:00
#PBS -q short_cpuQ


module load mpich-3.2
mpicc -g -Wall -o MPI_parallel MPI_parallel.c -std=c99 -lm 
mpirun.actual -n 4 ./MPI_parallel

