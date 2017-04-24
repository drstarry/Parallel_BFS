#!/bin/bash

#SBATCH --time=1:00:00      # walltime, abbreviated by -t
#SBATCH --nodes=2           # number of cluster nodes, abbreviated by -N
#SBATCH -o out/task1-%j.o%N # name of the stdout, using the job number (%j) and 
#                           # the first node (%N)
#SBATCH -e out/task1-%j.e%N # name of the stderr, using job and first node values
#SBATCH --ntasks=16         # number of MPI tasks, abbreviated by -n (MAX = 28)
#
## additional information for allocated clusters
#SBATCH --account=soc-kp   # account - abbreviated by -A
#SBATCH --partition=soc-kp # partition, abbreviated by -p

## run the program
## mpirun -np $SLURM_NTASKS ./task1_mpi
## OMP_NUM_THREADS=$SLURM_NTASKS ./task1_omp

OMP_NUM_THREADS=2 mpirun -np 2  time -f "%e MPI: 2  OMP: 2 nodes: 2 - cen 128"  ./cen 128
OMP_NUM_THREADS=2 mpirun -np 4  time -f "%e MPI: 4  OMP: 2 nodes: 2 - cen 256"  ./cen 256
OMP_NUM_THREADS=2 mpirun -np 8  time -f "%e MPI: 8  OMP: 2 nodes: 2 - cen 512"  ./cen 512
OMP_NUM_THREADS=2 mpirun -np 16 time -f "%e MPI: 16 OMP: 2 nodes: 2 - cen 1024" ./cen 1024

OMP_NUM_THREADS=2 mpirun -np 2  time -f "%e MPI: 2  OMP: 2 nodes: 2 - cen 512" ./cen 512
OMP_NUM_THREADS=2 mpirun -np 4  time -f "%e MPI: 4  OMP: 2 nodes: 2 - cen 512" ./cen 512
OMP_NUM_THREADS=2 mpirun -np 8  time -f "%e MPI: 8  OMP: 2 nodes: 2 - cen 512" ./cen 512
OMP_NUM_THREADS=2 mpirun -np 16 time -f "%e MPI: 16 OMP: 2 nodes: 2 - cen 512" ./cen 512
