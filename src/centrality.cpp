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
#include <stack>
#include <queue>
#include <algorithm>

#include "graph.h"

#define ROOT_RANK 0

// implements the parallelized version of brandes betweeness centrality
// algorithm from:
// http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.85.5626&rep=rep1&type=pdf
Graph::centrality_T parallelBrandesCentrality(Graph graph, int mpi_rank,
                                             int mpi_size, int omp_size) {
  Graph::centrality_T centrality;
  std::vector<int> shortestPath;
  std::vector<int> distance;
  std::vector<float> pairDependency;

  // translates all vertex ids into a incrementing indices for an array
  int i = 0;
  std::map<int, int> iToG; // translator
  std::map<int, int> gToI; // translator
  for (Graph::adjList_T::iterator it = graph.adjacencyList.begin();
       it != graph.adjacencyList.end(); it++) {
    gToI[it->first] = i; // map graph vertex ids to vector indices
    iToG[i] = it->first; // map vector indices to graph vertex ids
    i++;
  }

  int numVertices = graph.getNumVertices();

  #pragma omp parallel for
  for (int j = 0; j < numVertices; j++) {
    // map each index value back to the graph vertex id
    centrality.insert(std::pair<int, float>(iToG[j], 0.0));
  }

  #pragma omp parallel for
  for (int s = 0; s < numVertices; s++) {
    std::stack<int> visited;
    std::queue<int> bfsQueue;
    std::vector<std::vector<int> > predecessors(numVertices);

    for (i = 0; i < numVertices; i++) {
      shortestPath.push_back(0);
      distance.push_back(-1);
    }

    // s is index. use iToG[s] to get graph vertex id
    shortestPath[s] = -1;
    distance[s] = 0;
    bfsQueue.push(s);

    while (!bfsQueue.empty()) {
      // v is index. use iToG[v] to get graph vertex
      int v = bfsQueue.front();
      bfsQueue.pop();
      visited.push(v);

      std::vector<int> neighbors = graph.getNeighbors(iToG[v]);
      #pragma omp parallel for
      for (i = 0; i < neighbors.size(); i++) {
        // neighbors[i] is graph vertex. use gToI[...] to get index
        int w = gToI[neighbors[i]];
        if (distance[w] < 0) {
          bfsQueue.push(w);
          distance[w] = distance[v] + 1;
        }

        if (distance[w] == distance[v] + 1) {
          shortestPath[w] = shortestPath[w] + shortestPath[v];
          predecessors[w].push_back(v);
        }
      }
    }

    for (i = 0; i < numVertices; i++) {
      pairDependency.push_back(0.0);
    }

    while (!visited.empty()) {
      int w = visited.top();
      visited.pop();

      std::vector<int> preds = predecessors[w];
      for (i = 0; i < preds.size(); i++) {
        int v = preds[i];
        pairDependency[v] = pairDependency[v] +
                            ((float)shortestPath[v] / (float)shortestPath[w]) *
                            (1.0 + pairDependency[w]);
      }

      if (w != s) {
        centrality[iToG[w]] = centrality[iToG[w]] + pairDependency[w];
      }
    }
  }

  return centrality;
}



// orders the centrality metric map by vertices with the highest centrality,
// and prints them out in that order
std::pair<float, int> flipPair(const std::pair<int, float> &p) {
  return std::pair<float, int>(p.second, p.first);
}
void printMostCentral(Graph::centrality_T centrality, int mpi_rank) {
  std::multimap<float, int> sortedCent;
  std::transform(centrality.begin(), centrality.end(),
                 std::inserter(sortedCent, sortedCent.begin()), flipPair);

  // print out results of centrality ranking
  for (std::multimap<float, int>::reverse_iterator it = sortedCent.rbegin();
       it != sortedCent.rend(); it++) {
    printf("r: %d vertex: %d  \tbetweeness centrality metric: %0.02f\n",
           mpi_rank + 1, it->first, it->second);
  }
}



/**
 * kicks it all off
 */
int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int mpi_rank, mpi_size, omp_size, DIM = 32;
  if (argc == 2) {
    // read integer from command line argument
    sscanf(argv[1], "%d", &DIM);
  }

  // get the size and rank for mpi and omp
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  omp_size = omp_get_max_threads();

  // initialize a graph
  //Graph graph = Graph(mpi_rank, DIM);
  Graph graph = Graph(ROOT_RANK, DIM);

  // read the whole graph into memory for each processor
  //graph.buildGraphFromFile("data/facebook_combined.txt");
  graph.buildRandomGraph();

  // figure out centrality metrics for all vertices
  Graph::centrality_T centrality = parallelBrandesCentrality(graph, mpi_rank,
                                            mpi_size, omp_size);

  if (mpi_rank == ROOT_RANK) {
    graph.writeAsDotGraph(centrality);
    printMostCentral(centrality, mpi_rank);
  }

  // wait until everyone is done
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Finalize();
  return 0;
}
