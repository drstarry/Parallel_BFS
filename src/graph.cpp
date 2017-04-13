// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#include <graph.h>

// constructor to build a new vertex with 0 data
Vertex::Vertex(void) {
  Vertex(0);
}

// constructor to build a new vertex with some data
Vertex::Vertex(int data) :
  data(data)
{
  this->id = this->count++;
}

// returns the number of edges this vertex has
int Vertex::getNumNeighbors() {
  return this->neighbors.size();
}

// create an edge between an existing vertex and this vertex
Vertex Vertex::addNeighbor(Vertex node) {
  this->neighbors.push_bash(node);
  return node;
}

// create an edge between a new vertex and this vertex
Vertex Vertex::addNewNeighbor() {
  Vertex newNode = Vertex();
  this->neighbors.push_bash(newNode);
  return newNode;
}

// compares two vertices to see if they are the same vertex
bool Vertex::equals(Vertex node) {
  return this->id == node.id &&
         this->getNumNeighbors() == node.getNumNeighbors();
}

// returns if the provided vertex is one of this vertex's neighbors
bool Vertex::isNeighbor(Vertex node) {
  for (Vertex neighbor : this->neighbors) {
    if (node.equals(neighbor)) {
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
  for (Vertex node : this->neighbors) {
    printf("%d -- %d;\n", this->id, node.id);
  }
  printf("}\n");
}

void Vertex::buildRandomGraph(int maxDepth) {
  int i, depth = 0; r1 = 4, r2 = 5; // TODO: r1 should be random < maxDepth
  Vertex node;
  vector<Vertex> toProcess;
  vector<Vertex> processed;
  processed.push_back(this);

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
        toProcess.insert(toProcess.begin(), node.AddNewNeighbor());
      } else { // create edge to an existing processed node
        node.AddNeighbor(processed[r % processed.size()]);
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
  if (this.vertices.back.isNeighbor(node)) {
    // the provided vertex has no edge to the last vertex in the path
    return false;
  }

  this->vertices.push_back(node);
  return true;
}

// returns the number of edges in this path
int Path::getSize(void) {
  return this->vertices - 1;
}
