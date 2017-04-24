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
#include <string>

#define MAX_GRAPH_SIZE 1000


class Graph {
public:
		const int rank; // the processor rank using this graph
		const int size; // size x size matrix
		int numVertices;
		int matrix[MAX_GRAPH_SIZE*MAX_GRAPH_SIZE]; // initializes to all 0s
		Graph(int, int); // implicit destructor

		void addEdge(int, int);
		bool isNeighbor(int, int);
		int getNumNeighbors(int);
		std::vector<int> getNeighbors(int);
		void writeAsDotGraph(void);
		void printAsMatrix(void);
		void buildRandomGraph(void);
		void buildGraphFromFile(const char *);
		void buildSubGraphFromFile(const char *, int, int);
};


//class Path {
//public:
//		std::vector<int> pres;
//		Path(int);
//		void set(int, int);
//		int* frequency();
//};

#endif
