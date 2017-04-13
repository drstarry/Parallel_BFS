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

// TODO: basic algorithm for finding centrality!
//
// short_path_counts = {0} // length N
// for (i = 0; i < ((n - 1) * (n - 1) / 2); i++) {
//   run the algorithm from the paper
//   for each of the vertices returned in the shortest path {
//     short_path_counts[vertex]++;
//   }
// }
// find the index with the highest count

// TODO
Path shortestPath(int start, int end) {
  Path shortest = Path();
  return shortest;
}

// TODO
int bfs(Graph graph) {
  return 0;
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

  Graph graph = Graph(DIM); // build a graph of DIM x DIM size
  if (mpi_rank == ROOT_RANK) {
    graph.buildRandomGraph();
    graph.printAsDotGraph();

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

  //bfs(graph);

  // wait until everyone is done
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Finalize();
  return 0;
}
