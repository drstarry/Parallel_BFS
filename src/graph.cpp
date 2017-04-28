// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#include <iostream>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <string.h>
#include <fstream>
#include <iterator>
#include <limits>

#include "graph.h"
#include "utils.h"


#ifdef _WIN32
#define safeRand rand
#else
#define safeRand rand_r
#endif


// constructs a new graph using a dynamic adjacency list
Graph::Graph(int rank) : rank(rank + 1), size(1000) {
}


// constructs a new graph using a dynamic adjacency list
Graph::Graph(int rank, int size) : rank(rank + 1), size(size) {
}


// adds an edge between two vertices
void Graph::addEdge(int vertex1, int vertex2) {
  if (vertex1 == vertex2) {
    // these are the same vertices. no self edges are allowed.
    return;
  }

  if (this->isNeighbor(vertex1, vertex2)) {
    // these two vertices are already neighbors
    return;
  }

  this->adjacencyList[vertex1].push_back(vertex2);
  this->adjacencyList[vertex2].push_back(vertex1);
}


// returns if the two vertices have a connecting edge
bool Graph::isNeighbor(int vertex1, int vertex2) {
  std::vector<int> neighbors = this->getNeighbors(vertex1);
  for (int i = 0; i < neighbors.size(); i++) {
    if (neighbors[i] == vertex2) {
      return true;
    }
  }
  return false;
}


// returns the number of vertices the provided vertex is adjacent to
int Graph::getNumNeighbors(int vertex) {
  return this->adjacencyList[vertex].size();
}


// returns a vector of the vertices connected to the provided vertex
std::vector<int> Graph::getNeighbors(int vertex) {
  return this->adjacencyList[vertex];
}


// returns the number of vertices that exist in the graph
int Graph::getNumVertices(void) {
  return this->adjacencyList.size();
}


// writes a dot graph representation of this graph to stdout
void Graph::writeAsDotGraph(void) {
  int key, i;
  char filename[13];
  char rankStr[2];
  strcpy(filename, "graph-");
  sprintf(rankStr, "%d", this->rank);
  strcat(filename, rankStr);
  strcat(filename, ".dot");

  std::ofstream graphFile(filename);
  if (graphFile.is_open()) {
    graphFile << "graph graphname {\n";
    for (Graph::adjListT::iterator it = this->adjacencyList.begin();
         it != this->adjacencyList.end(); it++) {
      key = it->first;
      std::vector<int> neighbors = this->getNeighbors(key);
      for (i = 0; i < neighbors.size(); i++) {
        graphFile << key << " -- " << neighbors[i] << ";\n";
      }
    }
    graphFile << "}\n";
    graphFile.close();
  } else {
    printf("Error opening file!\n");
  }
}


// sets a bunch of edges randomly in a graph
void Graph::buildRandomGraph(void) {
  int i, j, r = 0;
  unsigned int seed = time(NULL) / this->rank;
  for (i = 0; i < this->size; i++) {
    for (j = 0; j < this->size; j++) {
      r = (safeRand(&seed) % this->size);
      if (r == 0) { // ~20% of adding an edge
        this->addEdge(i, j);
      }
    }
  }
}


// builds up the graph by reading in a text file. assumes text file is in the
// format where each line only contains <vertex vertex>. Where each vertex is
// an integer, and each line represent an edge between each vertex.
void Graph::buildGraphFromFile(const char *filename) {
  std::string line;
  std::ifstream dataFile(filename);
  int v1, v2;

  while (std::getline(dataFile, line)) {
    std::vector<std::string> parts = split(line, ' ');
    v1 = atoi(parts[0].c_str());
    v2 = atoi(parts[1].c_str());
    this->addEdge(v1, v2);
  }
}


// builds up the graph by reading in a text file. assumes text file is in the
// format where each line only contains <vertex vertex>. Where each vertex is
// an integer, and each line represent an edge between each vertex. This
// particular function will calculate an offset in the full graph file and only
// build part of the graph in memory depending on the processor rank and the
// total size. This is naive and slow.
void Graph::buildSubGraphFromFile(const char *filename, int rank, int size) {
  std::string line;
  std::ifstream dataFile(filename);
  int v1, v2;
  int lineCount = 0;

  // count the number of lines and the figure out the offset in the vertex ids
  while (std::getline(dataFile, line)) {
    lineCount++;
  }

  // only care to process the rankth 1/size chunk of the total file
  int chunkSize = (lineCount / size);
  int chunkBeg = rank * chunkSize;
  int chunkEnd = chunkBeg + chunkSize - 1;

  // from http://stackoverflow.com/a/5207600
  // seek to the beginning of this files chunk
  dataFile.clear();
  dataFile.seekg(0, std::ios::beg);
  for (int i = 0; i < chunkBeg - 1; i++){
    dataFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }

  printf("lineCount: %d\n", lineCount);
  printf("chunkBeg: %d, chunkEnd %d\n", chunkBeg, chunkEnd);

  // build graph from the specific chunk that this process should handle
  while (std::getline(dataFile, line) && chunkBeg < chunkEnd) {
    std::vector<std::string> parts = split(line, ' ');
    v1 = atoi(parts[0].c_str());
    v2 = atoi(parts[1].c_str());

    this->addEdge(v1, v2);
    chunkBeg++;
  }
}
