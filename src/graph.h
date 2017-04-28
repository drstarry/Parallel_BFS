// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <map>


// builds a Graph. G = (V, E). Represented internally using adjacency list
class Graph {
  public:
    typedef std::map<int, std::vector<int> > adjListT;

    const int rank; // the processor rank using this graph
    const int size; // for random graph generation, max size
    adjListT adjacencyList;
    Graph(int); // implicit destructor
    Graph(int, int);

    void addEdge(int, int);
    bool isNeighbor(int, int);
    int getNumNeighbors(int);
    std::vector<int> getNeighbors(int);
    int getNumVertices(void);
    void writeAsDotGraph(void);
    void buildRandomGraph(void);
    void buildGraphFromFile(const char *);
    void buildSubGraphFromFile(const char *, int, int);
};


#endif
