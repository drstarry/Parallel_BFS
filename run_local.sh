#!/bin/bash

echo "MPI: 2  OMP: 2 nodes: 1 - bfs 256"
command time -p (OMP_NUM_THREADS=2 mpirun -np 2  ./bfs 128) 2>&1
echo "MPI: 4  OMP: 2 nodes: 1 - bfs 512"
command time -p (OMP_NUM_THREADS=2 mpirun -np 4  ./bfs 256) 2>&1
echo "MPI: 8  OMP: 2 nodes: 1 - bfs 1024"
command time -p (OMP_NUM_THREADS=2 mpirun -np 8  ./bfs 512) 2>&1
echo "MPI: 16 OMP: 2 nodes: 1 - bfs 2048"
command time -p (OMP_NUM_THREADS=2 mpirun -np 16 ./bfs 1024) 2>&1

echo ""
echo ""

echo "MPI: 2  OMP: 2 nodes: 1 - bfs 512"
command time -p (OMP_NUM_THREADS=2 mpirun -np 2  ./bfs 512) 2>&1
echo "MPI: 4  OMP: 2 nodes: 1 - bfs 512"
command time -p (OMP_NUM_THREADS=2 mpirun -np 4  ./bfs 512) 2>&1
echo "MPI: 8  OMP: 2 nodes: 1 - bfs 512"
command time -p (OMP_NUM_THREADS=2 mpirun -np 8  ./bfs 512) 2>&1
echo "MPI: 16 OMP: 2 nodes: 1 - bfs 512"
command time -p (OMP_NUM_THREADS=2 mpirun -np 16 ./bfs 512) 2>&1
