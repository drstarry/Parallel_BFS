// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#include "graph.h"
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#ifdef _WIN32
#define safeRand rand
#else
#define safeRand rand_r
#endif


// NOTE: size must be less than MAX_GRAPH_SIZE
// constructs a new undirected graph represented using a symmetric sparse
// adjacency matrix
Graph::Graph(int size) :
  size(size) {
}


// adds an edge between two vertices
void Graph::addEdge(int vertex1, int vertex2) {
  if (this->getNumNeighbors(vertex1) == 0) {
    // vertex1 had no neighbors before, so the vertex didn't exist
    this->numVertices++;
  }
  if (this->getNumNeighbors(vertex2) == 0) {
    // vertex2 had no neighbors before, so the vertex didn't exist
    this->numVertices++;
  }

  if (vertex1 != vertex2) {
    this->matrix[vertex1][vertex2] = 1;
    this->matrix[vertex2][vertex1] = 1;
  }
}


// returns if the two vertices have a connecting edge
bool Graph::isNeighbor(int vertex1, int vertex2) {
  return this->matrix[vertex1][vertex2] == 1;
}


// gets the number of outgoing edges a specific
//
//        1 2 3 4 5
//      -------------
//    1 | 0 1 0 1 0 |
//    2 | 1 0 0 1 0 |
//    3 | 0 0 0 1 1 |
//    4 | 1 1 1 0 1 |
//    5 | 0 0 1 1 0 |
//      -------------
//
int Graph::getNumNeighbors(int vertex) {
  int i, neighborCount = 0;
  for (i = 0; i < this->size; i++) {
    if (this->isNeighbor(vertex, i)) {
      neighborCount++;
    }
  }
  return neighborCount;
}


// returns a vector of the vertices connected to the provided vertex
std::vector<int> Graph::getNeighbors(int vertex) {
  std::vector<int> neighbors;
  for (int i = 0; i < this->size; i++) {
    if (this->isNeighbor(vertex, i)) {
      neighbors.push_back(i);
    }
  }
  return neighbors;
}


// writes a dot graph representation of this graph to stdout
void Graph::printAsDotGraph(void) {
  int i, j;
  printf("graph graphname {\n");
  for (i = 0; i < this->size; i++) {
    for (j = 0; j < i; j++) {
      if (this->isNeighbor(i, j)) {
        printf("%d -- %d;\n", i, j);
      }
    }
  }
  printf("}\n");
}


// writes a matrix representation of this graph to stdout
void Graph::printAsMatrix(void) {
  int i, j;
  for (i = 0; i < this->size; i++) {
    for (j = 0; j < this->size; j++) {
      printf(" %d", this->matrix[i][j]);
    }
    printf("\n");
  }
}


// sets a bunch of edges randomly in a graph
void Graph::buildRandomGraph(void) {
  int i, j, r = 0;
  unsigned int seed = time(NULL);
  for (i = 0; i < this->size; i++) {
    for (j = 0; j < this->size; j++) {
      r = (safeRand(&seed) % 5);
      if (r == 0) { // ~20% of adding an edge
        this->addEdge(i, j);
      }
    }
  }
}


// constructor to build a path of edges through a graph
Path::Path(void) {
}


// adds a vertex to the path being built up
void Path::addVertex(int vertex) {
  this->vertices.push_back(vertex);
}


// returns the number of edges in this path
int Path::getSize(void) {
  return this->vertices.size() - 1;
}


// writes a dot graph representation of this graph to stdout
void Path::printAsDotGraph(void) {
  printf("graph graphname {\n");
  for (int i = 1; i < this->vertices.size(); i++) {
    printf("%d -- %d;\n", this->vertices[i-1], this->vertices[i]);
  }
  printf("}\n");
}
