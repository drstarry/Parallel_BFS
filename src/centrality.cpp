// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#include <cstring>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <deque>
#include <typeinfo>

#include "graph.h"

using namespace std;

#define ROOT_RANK 0

int mpi_rank, mpi_size, block, DIM, sub_DIM;


// a source node, find shortest path
int* shortestPath(int* graph, int start) {
	// local and global vertexs
	int* vertexes = new int[DIM];
	memset(vertexes, 0, DIM*sizeof(int));
	vertexes[start] = 1;
	int* local_ver = new int[sub_DIM];

	// local and global distances
	int* glob_dist = new int[DIM];
	memset(glob_dist, -1, DIM*sizeof(int));
	glob_dist[start] = 0;
	int* local_dist = new int[sub_DIM];

	// local and global reached signal
	int* glob_reached = new int[DIM];
	memset(glob_reached, 0, DIM*sizeof(int));
	glob_reached[start] = 1;
	int* local_reached = new int[sub_DIM];


	int idx;
	int all_cnt = 1, new_cnt = 0;
	int round = 0;
	bool stop = false;
	for (int round = 1; round <= DIM; round++) {
		if (all_cnt == 0) {
			// cout << "all count = " << all_cnt << endl;
			break;
		}

		// send vertexes to each process
		MPI_Scatter(vertexes, sub_DIM, MPI_INT, local_ver,
					sub_DIM, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
		// send distances to each process
		MPI_Scatter(glob_dist, sub_DIM, MPI_INT, local_dist,
					sub_DIM, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
		// send reached to each process
		MPI_Scatter(glob_reached, sub_DIM, MPI_INT, local_reached,
					sub_DIM, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);

		// openmp
		for (int i = 0; i < sub_DIM; i++) {
			// #openmp parallel for
			for (int j = 0; j < DIM; j++) {
				idx = DIM * i + j;
				if (graph[idx] == 1 && vertexes[j] == 1 && local_reached[i] == 0) {
					local_reached[i] = 1;
					local_ver[i] = 1;
					local_dist[i] = round;
					// cout << "dist:" << local_dist[i] << endl;
					new_cnt += 1;
				}
			}
		}

		for (int i = 0; i < DIM; i++) {
			// cout << glob_reached[i] << " ";
		}
		// cout << endl;
		MPI_Gather(local_ver, sub_DIM, MPI_INT,
				   vertexes, sub_DIM, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
		MPI_Gather(local_dist, sub_DIM, MPI_INT,
				   glob_dist, sub_DIM, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
		MPI_Gather(local_reached, sub_DIM, MPI_INT,
				   glob_reached, sub_DIM, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);

		// gather if all process finished
		MPI_Gather(&new_cnt, 1, MPI_INT, &all_cnt, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
		MPI_Bcast(&all_cnt, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);

		if (mpi_rank == ROOT_RANK) {
			// cout << "******round " << round << endl;
			for (int j  = 0; j < DIM; j++) {
				// cout << glob_dist[j] << ", ";
			}
			// cout << endl;
			for (int j  = 0; j < DIM; j++) {
				// cout << vertexes[j] << ", ";
			}
			// cout << "\n********\n";
		}

		new_cnt = 0;
	}

	delete[] local_dist, local_reached, local_ver, glob_reached, vertexes;

	return glob_dist;
}

// count all pair to compute centrality
//void counting() {
//
//}

//int printSequentialBfs(Graph graph) {
//	int i, j, root, currentVertex;
//	bool visited[graph.numvertexes];
//	std::deque<int> queue;
//
//	// initialize visited array to false
//	for (i = 0; i < graph.numvertexes; i++) {
//		visited[i] = false;
//	}
//
//	// find the first node in the matrix. use this as the root node
//	root = -1;
//	for (i = 0; i < graph.size && root == -1; i++) {
//		for (j = 0; j < graph.size && root == -1; j++) {
//			if (graph.isNeighbor(i, j)) {
//				// an edge exists between these two vertexes, so obviously the vertexes
//				// exist and we can use one of them as the root
//				root = i;
//			}
//		}
//	}
//
//	// we have visited the root node now
//	visited[root] = true;
//	queue.push_back(root);
//
//	// does a bfs through the adjacency matrix and prints out a dot graph
//	// representation of the search path as a directed tree
//	printf("digraph bfs {\n");
//	while (!queue.empty()) {
//		currentVertex = queue.front();
//		queue.pop_front();
//
//		std::vector<int> neighbors = graph.getNeighbors(currentVertex);
//		for (i = 0; i < neighbors.size(); i++) {
//			j = neighbors[i];
//			if (!visited[j]) {
//				visited[j] = true;
//				queue.push_back(j);
//				printf("%d -> %d;\n", currentVertex, j);
//			}
//		}
//	}
//	printf("}\n");
//
//	return 0;
//}

/**
 * kicks it all off
 */
int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);

	DIM = 10;
	if (argc == 2) {
		// read integer from command line argument
		sscanf(argv[1], "%d", &DIM);
	}

	int i, omp_size, offset;
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
	omp_size = omp_get_max_threads();
	sub_DIM = DIM / mpi_size; // sub dim in each process
	block = DIM*sub_DIM;

	Graph graph = Graph(DIM); // initialize a graph of DIM x DIM size

	// get sub matrix
	int* local_graph = new int[block];
	if (mpi_rank == ROOT_RANK) {
    graph.buildGraphFromFile("data/facebook_combined.txt");
		//graph.buildRandomGraph(); // randomly adds edges
		//graph.printAsMatrix();
    graph.printAsDotGraph();
	}

	// send chunks to each process
	MPI_Scatter(graph.matrix, block, MPI_INT, local_graph,
	 			block, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);

	for (i = 0; i < DIM; i++) {
		// cout << "------source " << i << endl;
		int* dist = shortestPath(local_graph, i);
		// cout << "------" << endl;
		MPI_Barrier(MPI_COMM_WORLD);
	}

	// wait until everyone is done
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();

	delete[] local_graph;
	return 0;
}
