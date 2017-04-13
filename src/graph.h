// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

class Vertex {
  private:
    static int globalCount;
    int id;

  public:
    int data;
    std::vector<Vertex> neighbors;
    Vertex(void); // implicit destructor
    Vertex(int);

    // assignment operator
    Vertex& operator=(const Vertex&);

    int getNumNeighbors(void);
    Vertex addNeighbor(Vertex);
    Vertex addNewNeighbor(void);
    bool equals(Vertex);
    bool isNeighbor(Vertex);
    void printGraphAsDot(void);
    void buildRandomGraph(int);
};

class Path {
  public:
    std::vector<Vertex> vertices;
    Path(void); // implicit destructor
    bool addVertex(Vertex);
    int getSize(void);
    void printPath(void);
};

#endif
