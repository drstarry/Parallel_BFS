// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#include "graph.h"
#include <vector>
#include <cstdio>

int Vertex::globalCount = 0;

// constructor to build a new vertex with 0 data
Vertex::Vertex(void) :
  data(0), id(++globalCount) {
}

// constructor to build a new vertex with some data
Vertex::Vertex(int data) :
  data(data), id(++globalCount) {
}

// assignment operator. used when pulling stuff out of a vector
Vertex& Vertex::operator=(const Vertex& other) {
  this->id = other.id;
  this->data = other.data;
  this->neighbors.clear();
  for (int i = 0; i < other.neighbors.size(); i++) {
    this->neighbors.push_back(other.neighbors[i]);
  }
  return *this;
}

// returns the number of edges this vertex has
int Vertex::getNumNeighbors() {
  return this->neighbors.size();
}

// create an edge between an existing vertex and this vertex
Vertex Vertex::addNeighbor(Vertex node) {
  this->neighbors.push_back(node);
  return node;
}

// create an edge between a new vertex and this vertex
Vertex Vertex::addNewNeighbor() {
  Vertex newNode = Vertex();
  this->neighbors.push_back(newNode);
  return newNode;
}

// compares two vertices to see if they are the same vertex
bool Vertex::equals(Vertex node) {
  return this->id == node.id &&
         this->getNumNeighbors() == node.getNumNeighbors();
}

// returns if the provided vertex is one of this vertex's neighbors
bool Vertex::isNeighbor(Vertex node) {
  for (int i = 0; i < this->getNumNeighbors(); i++) {
    if (node.equals(this->neighbors[i])) {
      return true;
    }
  }
  return false;
}

// writes a dot graph representation of this graph to stdout
// TODO: right now this just gets the immediate neighbors, not the full graph
void Vertex::printGraphAsDot(void) {
  printf("graph graphname {\n");
  // use bfs!
  for (int i = 0; i < this->getNumNeighbors(); i++) {
    printf("%d -- %d;\n", this->id, this->neighbors[i].id);
  }
  printf("}\n");
}

void Vertex::buildRandomGraph(int maxDepth) {
  int i, depth = 0, r1 = 4, r2 = 5; // TODO: r1 should be random < maxDepth
  Vertex node;
  std::vector<Vertex> toProcess;
  std::vector<Vertex> processed;
  processed.push_back(*this);

  // initialize root node with some neighbors
  for (i = 0; i < r1; i++) {
    // add neighbors to front of queue as they are added as neighbors
    toProcess.insert(toProcess.begin(), this->addNewNeighbor());
  }

  while (toProcess.size() > 0 && depth < maxDepth) {
    node = toProcess.back(); // get element to add neighbors to
    toProcess.pop_back();
    r1 = 3; // TODO: r1 should be random < maxDepth / 2
    for (i = 0; i < r1; i++) {
      r2 = 4; // TODO: r2 should be random < maxDepth
      if (r2 < maxDepth / 3) { // with 1/3 chance, create edge to new node
        toProcess.insert(toProcess.begin(), node.addNewNeighbor());
      } else { // create edge to an existing processed node
        node.addNeighbor(processed[r2 % processed.size()]);
      }
    }
    processed.push_back(node);
    depth++;
  }
}

// constructor to build a path of edges through a graph
Path::Path(void) {
}

// 
bool Path::addVertex(Vertex node) {
  if (!this->vertices.back().isNeighbor(node)) {
    // the provided vertex has no edge to the last vertex in the path
    return false;
  }

  this->vertices.push_back(node);
  return true;
}

// returns the number of edges in this path
int Path::getSize(void) {
  return this->vertices.size() - 1;
}
