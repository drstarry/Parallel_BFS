// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#include "graph.h"
#include <iostream>
#include <mpi.h>
#include <omp.h> 

#define ROOT_RANK 0

// TODO
int shortestPath(Vertex start, Vertex end) {
  return -1;
}

// TODO
Vertex bfs(Vertex root) {
  return root;
}

/**
 * kicks it all off
 */
int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int DIM = 10;
  if (argc == 2) {
    // read integer from command line argument
    sscanf(argv[1], "%d", &DIM);
  }

  int i, mpi_rank, mpi_size, omp_size, block, offset;
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  omp_size = omp_get_max_threads();

  Vertex root = Vertex();
  if (mpi_rank == ROOT_RANK) {
    root.buildRandomGraph(7);
    root.printGraphAsDot();

    //matrix = malloc(sizeof(double) * MAT_SIZE);
    //memset(matrix, 0, sizeof(double) * MAT_SIZE);
  
    //// root initializes the whole matrix
    //init_matrix(matrix, DIM, MAT_SIZE);
    //for (i = 1; i < mpi_size; ++i) {
    //  // sends off chunks to each sub process
    //  MPI_Send(&matrix[offset], block, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
    //  offset += block;
    //}
  } else {
    //matrix = malloc(sizeof(double) * block);
    //memset(matrix, 0, sizeof(double) * block);
    //MPI_Recv(matrix, block, MPI_DOUBLE, ROOT_RANK, 0, MPI_COMM_WORLD, NULL);
  }

  //bfs(vertex);

  // wait until everyone is done
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Finalize();
  return 0;
}
