#!/bin/bash

echo "MPI: 2  OMP: 2 nodes: 1 - centrality 256"
OMP_NUM_THREADS=1 command time -p mpirun -np 1  ./centrality 4 2>&1
#OMP_NUM_THREADS=2 command time -p mpirun -np 2  ./centrality 128 2>&1
# echo "MPI: 4  OMP: 2 nodes: 1 - centrality 512"
# command time -p (OMP_NUM_THREADS=2 mpirun -np 4  ./centrality 256) 2>&1
# echo "MPI: 8  OMP: 2 nodes: 1 - centrality 1024"
# command time -p (OMP_NUM_THREADS=2 mpirun -np 8  ./centrality 512) 2>&1
# echo "MPI: 16 OMP: 2 nodes: 1 - centrality 2048"
# command time -p (OMP_NUM_THREADS=2 mpirun -np 16 ./centrality 1024) 2>&1
# 
# echo ""
# echo ""
# 
# echo "MPI: 2  OMP: 2 nodes: 1 - centrality 512"
# command time -p (OMP_NUM_THREADS=2 mpirun -np 2  ./centrality 512) 2>&1
# echo "MPI: 4  OMP: 2 nodes: 1 - centrality 512"
# command time -p (OMP_NUM_THREADS=2 mpirun -np 4  ./centrality 512) 2>&1
# echo "MPI: 8  OMP: 2 nodes: 1 - centrality 512"
# command time -p (OMP_NUM_THREADS=2 mpirun -np 8  ./centrality 512) 2>&1
# echo "MPI: 16 OMP: 2 nodes: 1 - centrality 512"
# command time -p (OMP_NUM_THREADS=2 mpirun -np 16 ./centrality 512) 2>&1
