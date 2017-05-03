// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#include <cstring>
#include <iostream>
#include <fstream>
#include <mpi.h>
#include <omp.h>
#include <stack>
#include <queue>
#include <algorithm>
#include <string>

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

  // translates all vertex ids into a incrementing indices for an array. this
  // is acceptable to do on each processor as the c++ std::map structure will
  // have a consistent ordering when being iterated through as per the standard
  int i = 0;
  std::map<int, std::string> iToG; // translator
  std::map<std::string, int> gToI; // translator
  for (Graph::adjList_T::iterator it = graph.adjacencyList.begin();
       it != graph.adjacencyList.end(); it++) {
    gToI[it->first] = i; // map graph vertex ids to vector indices
    iToG[i] = it->first; // map vector indices to graph vertex ids
    i++;
  }

  int numVertices = graph.getNumVertices();

  #pragma omp parallel for schedule(static) shared(centrality, iToG)
  for (int j = 0; j < numVertices; j++) {
    // map each index value back to the graph vertex id
    #pragma omp critical(centralityUpdate)
    {
    centrality.insert(std::pair<std::string, float>(iToG[j], 0.0));
    }
  }

  // only care to process the rankth 1/size chunk of the total graph
  int chunkSize = (numVertices / mpi_size);
  int chunkBeg = mpi_rank * chunkSize;
  int chunkEnd = chunkBeg + chunkSize - 1;

  #pragma omp parallel for schedule(static) default(none) \
          firstprivate(chunkBeg, chunkEnd, numVertices, i, iToG, gToI) \
          shared(graph, centrality, shortestPath, distance, pairDependency)
  for (int s = chunkBeg; s < chunkEnd; s++) {
    std::stack<int> visited;
    std::queue<int> bfsQueue;
    std::vector<std::vector<int> > predecessors(numVertices);

    for (i = 0; i < numVertices; i++) {
      #pragma omp critical(shortestPathUpdate)
      {
      shortestPath.push_back(0);
      }

      #pragma omp critical(distanceUpdate)
      {
      distance.push_back(-1);
      }
    }

    // s is index. use iToG[s] to get graph vertex id
    #pragma omp critical(shortestPathUpdate)
    {
    shortestPath[s] = -1;
    }

    #pragma omp critical(distanceUpdate)
    {
    distance[s] = 0;
    }
    bfsQueue.push(s);

    while (!bfsQueue.empty()) {
      // v is index. use iToG[v] to get graph vertex
      int v = bfsQueue.front();
      bfsQueue.pop();
      visited.push(v);
      std::vector<std::string> neighbors;

      #pragma omp critical(neighborsRead)
      {
      neighbors = graph.getNeighbors(iToG[v]);
      }

      #pragma omp parallel for schedule(static) firstprivate(v, neighbors) \
              shared(bfsQueue, shortestPath, predecessors)
      for (i = 0; i < neighbors.size(); i++) {
        int w, dw, dv;
        // neighbors[i] is graph vertex. use gToI[...] to get index
        w = gToI[neighbors[i]];

        #pragma omp atomic read
        dw = distance[w];
        if (dw < 0) {
          #pragma omp critical(bfsQueueUpdate)
          {
          bfsQueue.push(w);
          }
          #pragma omp critical(distanceUpdate)
          {
          distance[w] = distance[v] + 1;
          }
        }

        #pragma omp atomic read
        dw = distance[w];
        #pragma omp atomic read
        dv = distance[v];
        if (dw == dv + 1) {
          #pragma omp critical(shortestPathUpdate)
          {
          shortestPath[w] = shortestPath[w] + shortestPath[v];
          }
          #pragma omp critical(predecessorsUpdate)
          {
          predecessors[w].push_back(v);
          }
        }
      }
    }

    for (i = 0; i < numVertices; i++) {
      #pragma omp critical(pairDependencyUpdate)
      {
      pairDependency.push_back(0.0);
      }
    }

    while (!visited.empty()) {
      int w = visited.top();
      visited.pop();
      std::vector<int> preds;

      #pragma omp critical(predecessorsUpdate)
      {
      preds = predecessors[w];
      }

      for (i = 0; i < preds.size(); i++) {
        int v = preds[i];
        float shortest;
        #pragma omp critical(shortestPathUpdate)
        {
        shortest = ((float)shortestPath[v] / (float)shortestPath[w]);
        }

        #pragma omp critical(pairDependencyUpdate)
        {
        pairDependency[v] = pairDependency[v] + shortest * (1.0 + pairDependency[w]);
        }
      }

      if (w != s) {
        float pair;
        #pragma omp atomic read
        pair = pairDependency[w];

        #pragma omp critical(centralityUpdate)
        {
        centrality[iToG[w]] = centrality[iToG[w]] + pair;
        }
      }
    }
  }

  return centrality;
}



// orders the centrality metric map by vertices with the highest centrality,
// and prints them out in that order
std::pair<float, std::string> flipPair(const std::pair<std::string, float> &p) {
  return std::pair<float, std::string>(p.second, p.first);
}
void printMostCentral(Graph::centrality_T centrality, int mpi_rank) {
  std::multimap<float, std::string> sortedCent;
  std::transform(centrality.begin(), centrality.end(),
                 std::inserter(sortedCent, sortedCent.begin()), flipPair);

  // print out results of centrality ranking
  for (std::multimap<float, std::string>::reverse_iterator it = sortedCent.rbegin();
       it != sortedCent.rend(); it++) {
    printf("r: %d vertex: %s  \tbetweeness centrality metric: %0.02f\n",
           mpi_rank + 1, it->second.c_str(), it->first);
  }
}



// writes the centrality metrics for each vertex to a file
void writeCentrality(Graph::centrality_T centrality, int mpi_rank) {
  int i;
  char filename[18];
  char rankStr[2];
  strcpy(filename, "centrality-");
  sprintf(rankStr, "%d", mpi_rank);
  strcat(filename, rankStr);
  strcat(filename, ".txt");

  std::ofstream graphFile(filename);
  if (graphFile.is_open()) {
    for (Graph::centrality_T::iterator it = centrality.begin();
         it != centrality.end(); it++) {
      graphFile << it->first << " " << std::fixed << it->second << "\n";
    }
    graphFile.close();
  } else {
    printf("Error opening file!\n");
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

  // initialize a graph object on each processor
  Graph graph = Graph(ROOT_RANK, DIM);

  // read the whole graph into memory for each processor
  printf("building graph from file...\n");
  graph.buildGraphFromFile("data/gplus_combined.txt");
  //graph.buildGraphFromFile("data/facebook_combined.txt");
  //graph.buildRandomGraph();

  // figure out centrality metrics for part of the vertices depending on
  // mpi_rank
  printf("calculating centrality...\n");
  Graph::centrality_T centrality = parallelBrandesCentrality(graph, mpi_rank,
                                            mpi_size, omp_size);

  // only care to process the rankth 1/size chunk of the total graph
  int numVertices = graph.getNumVertices();
  int chunkSize = (numVertices / mpi_size);

  // chunkBeg -> chunkEnd will be the section of the map that this processor
  // already has
  int chunkBeg = mpi_rank * chunkSize;
  int chunkEnd = chunkBeg + chunkSize - 1;
  float *centralityMetrics = new float[numVertices];

  // each centrality metric that comes back is partial, so they need to all be
  // synchonized together before generating the dot graph
  if (mpi_rank == ROOT_RANK) {
    // need to gather all of the centrality metrics from non-root nodes, and
    // simply add them all into this centrality metric map
    for (int i = 0; i < mpi_size; i++) {
      if (i == ROOT_RANK) {
        continue;
      }

      // receive the centrality metrics from all other processes
      MPI_Recv(centralityMetrics, numVertices, MPI_FLOAT, i, 1, MPI_COMM_WORLD,
               NULL);

      // sum the received values together
      int j = 0;
      for (Graph::centrality_T::iterator it = centrality.begin();
           it != centrality.end(); it++) {
        centrality[it->first] += centralityMetrics[j++];
      }
    }

    printf("writing calculations to dot file...\n");
    graph.writeAsDotGraph(centrality);
  } else {
    // we know that the c++ std::map structure has a consistent iterator
    // ordering, so we can simply loop through it on each processor and not
    // worry about things being out of order
    int i = 0;
    for (Graph::centrality_T::iterator it = centrality.begin();
         it != centrality.end(); it++) {
      centralityMetrics[i++] = it->second;
    }

    // send the processed part of the centrality map to the root process to be
    // finalized and printed out
    MPI_Send(centralityMetrics, numVertices, MPI_FLOAT, ROOT_RANK, 1,
             MPI_COMM_WORLD);
  }

  //writeCentrality(centrality, mpi_rank);

  delete[] centralityMetrics;
  MPI_Finalize();
  return 0;
}
