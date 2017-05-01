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
#include <string>


// builds a Graph. G = (V, E). Represented internally using adjacency list
class Graph {
  public:
    typedef std::map<std::string, std::vector<std::string> > adjList_T;
    typedef std::map<std::string, float> centrality_T;


    const int rank; // the processor rank using this graph
    const int size; // for random graph generation, max size
    adjList_T adjacencyList;
    Graph(int); // implicit destructor
    Graph(int, int);

    void addEdge(std::string, std::string);
    bool isNeighbor(std::string, std::string);
    int getNumNeighbors(std::string);
    std::vector<std::string> getNeighbors(std::string);
    int getNumVertices(void);
    void writeAsDotGraph(void);
    void writeAsDotGraph(centrality_T);
    void buildRandomGraph(void);
    void buildGraphFromFile(const char *);
    void buildSubGraphFromFile(const char *, int, int);
};


#endif
