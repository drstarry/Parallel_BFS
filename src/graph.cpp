// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
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
void Graph::addEdge(std::string vertex1, std::string vertex2) {
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
bool Graph::isNeighbor(std::string vertex1, std::string vertex2) {
  std::vector<std::string> neighbors = this->getNeighbors(vertex1);
  for (int i = 0; i < neighbors.size(); i++) {
    if (neighbors[i] == vertex2) {
      return true;
    }
  }
  return false;
}


// returns the number of vertices the provided vertex is adjacent to
int Graph::getNumNeighbors(std::string vertex) {
  return this->adjacencyList[vertex].size();
}


// returns a vector of the vertices connected to the provided vertex
std::vector<std::string> Graph::getNeighbors(std::string vertex) {
  return this->adjacencyList[vertex];
}


// returns the number of vertices that exist in the graph
int Graph::getNumVertices(void) {
  return this->adjacencyList.size();
}


// write the graph as a dot graph with no weighting added
void Graph::writeAsDotGraph(void) {
  Graph::centrality_T centrality;
  for (Graph::adjList_T::iterator it = this->adjacencyList.begin();
       it != this->adjacencyList.end(); it++) {
    centrality.insert(std::pair<std::string, float>(it->first, 0.0));
  }
  this->writeAsDotGraph(centrality);
}


// writes a dot graph representation of this graph to a file
void Graph::writeAsDotGraph(Graph::centrality_T centrality) {
  int i;
  char filename[13];
  char rankStr[2];
  strcpy(filename, "graph-");
  sprintf(rankStr, "%d", this->rank);
  strcat(filename, rankStr);
  strcat(filename, ".dot");

  std::ofstream graphFile(filename);
  if (graphFile.is_open()) {
    graphFile << "graph graphname {\nrankdir=LR;\n";
    for (Graph::adjList_T::iterator it = this->adjacencyList.begin();
         it != this->adjacencyList.end(); it++) {
      graphFile << it->first << " [label=\"" << it->first << " - ";
      graphFile << std::fixed << centrality[it->first] << "\"";
      graphFile << " style=filled fillcolor=\"0.000 ";
      graphFile << std::fixed << centrality[it->first] / 500.0;
      graphFile << " 1.000\"];\n";
      graphFile << it->first << " -- {";
      std::vector<std::string> neighbors = it->second;
      for (i = 0; i < neighbors.size(); i++) {
        if (neighbors[i] > it->first) {
          graphFile << " " << neighbors[i];
        } else {
          // only want to display one edge between vertices
          graphFile << " /*" << neighbors[i] << "*/";
        }
      }
      graphFile << " };\n";
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
    for (j = this->size - 1; j >= 0; j--) {
      r = (safeRand(&seed) % this->size);
      if (r == 0) { // ~20% of adding an edge
        char vert1[10];
        char vert2[10];
        sprintf(vert1, "%d", i);
        sprintf(vert2, "%d", j);
        this->addEdge(std::string(vert1), std::string(vert2));
      }
    }
  }

  // make sure the graph is connected, so find path from 1 vertex to all others
  // using djikstra's algorithm
  std::map<std::string, int> dist;
  std::map<std::string, std::string> prev;
  std::queue<std::string> toVisit;
  std::string source;
  int unvisited = 999999999;

  for (Graph::adjList_T::iterator it = this->adjacencyList.begin();
       it != this->adjacencyList.end(); it++) {
    dist[it->first] = unvisited;
    prev[it->first] = "";
    toVisit.push(it->first);
  }

  // arbitrarily choose the first vertex as the source
  source = toVisit.front();
  dist[source] = 0;

  // visit all nodes to build up a distance map from source to everywhere
  while (!toVisit.empty()) {
    // normally use a priority queue to pick vertex with min dist, but meh
    std::string u = toVisit.front();
    toVisit.pop();

    // examine all of the neighbors
    std::vector<std::string> neighbors = this->getNeighbors(u);
    for (i = 0; i < neighbors.size(); i++) {
      std::string v = neighbors[i];
      if (prev[v] != "") {
        // if the node has already been visited, we don't care - skip
        continue;
      }

      // update distance for the unvisited neighbor nodes
      if (dist[v] > dist[u] + 1) {
        dist[v] = dist[u] + 1;
        prev[v] = u;
      }
    }
  }

  // add an edge to all nodes that have an 'unvisited' distance from source
  for (std::map<std::string, int>::iterator it = dist.begin(); it != dist.end(); it++) {
    if (it->second == unvisited) {
      this->addEdge(source, it->first);
      // so that the source isn't connected to lots of things, we know that if
      // we set a "new source" to the thing we just connected to the original
      // source, everything will become connected
      source = it->first;
    }
  }
}


// builds up the graph by reading in a text file. assumes text file is in the
// format where each line only contains <vertex vertex>. Where each vertex is
// an integer, and each line represent an edge between each vertex.
void Graph::buildGraphFromFile(const char *filename) {
  std::string line;
  std::ifstream dataFile(filename);
  std::string v1, v2;

  while (std::getline(dataFile, line)) {
    std::vector<std::string> parts = split(line, ' ');
    //v1 = stoll(parts[0].c_str());
    //v2 = stoll(parts[1].c_str());
    //this->addEdge(v1, v2);
    this->addEdge(parts[0], parts[1]);
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
  std::string v1, v2;
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
