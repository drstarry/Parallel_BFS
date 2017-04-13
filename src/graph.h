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
    static int count;

  public:
    int id;
    int data;
    std::vector<Vertex> neighbors;
    Vertex(void);
    Vertex(int);

    int getNumNeighbors(void);
    Vertex addNeighbor(Vertex)
    Vertex addNewNeighbor(void);
    bool equals(Vertex);
    bool isNeighbor(Vertex);
    void printGraphAsDot(void);
    void buildRandomGraph(int);
};

class Path {
  public:
    std::vector<Vertex> vertices;
    Path(void);
    bool addVertex(Vertex);
    int getSize(void);
    void printPath(void);
}

#endif
