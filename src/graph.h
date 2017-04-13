// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <stdint.h>

#define MAX_GRAPH_SIZE 999

class Graph {
  private:
    const int size;
    uint8_t graph[MAX_GRAPH_SIZE][MAX_GRAPH_SIZE]; // initializes to {0, 0, ...}

  public:
    Graph(int size);

    void addEdge(int, int);
    bool isNeighbor(int, int);
    int getNumNeighbors(int);
    std::vector<int> getNeighbors(int);
    void printAsDotGraph(void);
    void buildRandomGraph(void);
};

class Path {
  public:
    std::vector<int> vertices;
    Path(void); // implicit destructor
    void addVertex(int);
    int getSize(void);
    void printAsDotGraph(void);
};

#endif
