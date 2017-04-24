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
#include <climits>

#include "graph.h"
#include "utils.h"

using namespace std;


#ifdef _WIN32
#define safeRand rand
#else
#define safeRand rand_r
#endif


// NOTE: size must be less than MAX_GRAPH_SIZE
// constructs a new undirected graph represented using a symmetric sparse
// adjacency matrix
Graph::Graph(int rank, int size) : rank(rank + 1), size(size) {
}

// adds an edge between two vertices
void Graph::addEdge(int vertex1, int vertex2) {
	//// each vertex is represented as an index in the matrix. since each Graph
	//// only stores a portion of the full matrix, we can offset the vertex ids by
	//// this processor's rank
	//vertex1 = vertex1 / this->rank;
	//vertex2 = vertex2 / this->rank;

	if (vertex1 < 0 || vertex1 >= this->size || vertex2 < 0 || vertex2 >= this->size) {
		// vertex is outside the bounds

		return;
	}

	if (this->getNumNeighbors(vertex1) == 0) {
		// vertex1 had no neighbors before, so the vertex didn't exist
		this->numVertices++;
	}
	if (this->getNumNeighbors(vertex2) == 0) {
		// vertex2 had no neighbors before, so the vertex didn't exist
		this->numVertices++;
	}
	// cout << vertex1 << ", " << vertex2 << endl;
	if (vertex1 != vertex2) {
		this->matrix[vertex1*this->size+vertex2] = 1;
		this->matrix[vertex2*this->size+vertex1] = 1;
	}
}


// returns if the two vertices have a connecting edge
bool Graph::isNeighbor(int vertex1, int vertex2) {
	//// each vertex is represented as an index in the matrix. since each Graph
	//// only stores a portion of the full matrix, we can offset the vertex ids by
	//// this processor's rank
	//vertex1 = vertex1 / this->rank;
	//vertex2 = vertex2 / this->rank;

	return this->matrix[vertex1*this->size+vertex2] == 1;
}


// gets the number of outgoing edges a specific
//
//        1 2 3 4 5
//      -------------
//    1 | 0 1 0 1 0 |
//    2 | 1 0 0 1 0 |
//    3 | 0 0 0 1 1 |
//    4 | 1 1 1 0 1 |
//    5 | 0 0 1 1 0 |
//      -------------
//
int Graph::getNumNeighbors(int vertex) {
	int i, neighborCount = 0;
	for (i = 0; i < this->size; i++) {
		if (this->isNeighbor(vertex, i)) {
			neighborCount++;
		}
	}
	return neighborCount;
}


// returns a vector of the vertices connected to the provided vertex
std::vector<int> Graph::getNeighbors(int vertex) {
	std::vector<int> neighbors;
	for (int i = 0; i < this->size; i++) {
		if (this->isNeighbor(vertex, i)) {
			neighbors.push_back(i);
		}
	}
	return neighbors;
}


// writes a dot graph representation of this graph to stdout
void Graph::writeAsDotGraph(void) {
	int i, j;
	char filename[13];
	char rankStr[2];
	strcpy(filename, "graph-");
	sprintf(rankStr, "%d", this->rank);
	strcat(filename, rankStr);
	strcat(filename, ".dot");

	ofstream graphFile(filename);
	if (graphFile.is_open()) {
		graphFile << "graph graphname {\n";
		for (i = 0; i < this->size; i++) {
			for (j = 0; j < i; j++) {
				if (this->isNeighbor(i, j)) {
					// each vertex is represented as an index in the matrix. since each
					// Graph only stores a portion of the full matrix, we offset the vertex
					// ids by this processor's rank. so when printing them out, we need to
					// un-offset
					graphFile << i * this->size << " -- " << j * this->size << ";\n";
				}
			}
		}
		graphFile << "}\n";
		graphFile.close();
	} else {
		printf("Error opening file!\n");
	}
}


// writes a matrix representation of this graph to stdout
void Graph::printAsMatrix(void) {
	int i, j;
	for (i = 0; i < this->size; i++) {
		for (j = 0; j < this->size; j++) {
			printf(" %d", this->matrix[i*this->size+j]);
		}
		printf("\n");
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
	int minVertex = INT_MAX, maxVertex = INT_MIN;
	int lineCount = 0;

	// count the number of lines and the figure out the offset in the vertex ids
	while (std::getline(dataFile, line)) {
		std::vector<std::string> parts = split(line, ' ');
		v1 = atoi(parts[0].c_str());
		v2 = atoi(parts[1].c_str());
		minVertex = min(minVertex, min(v1, v2));
		maxVertex = max(maxVertex, max(v1, v2));
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
	printf("minVertex: %d, maxVertex: %d\n", minVertex, maxVertex);

	// build graph from the specific chunk that this process should handle
	while (std::getline(dataFile, line) && chunkBeg < chunkEnd) {
		std::vector<std::string> parts = split(line, ' ');
		v1 = atoi(parts[0].c_str()) - minVertex;
		v2 = atoi(parts[1].c_str()) - minVertex;

		if (v1 < 0) {
			printf("v1 - minVertex < 0! v1 = %d\n", v1 + minVertex);
		}
		if (v2 < 0) {
			printf("v2 - minVertex < 0! v2 = %d\n", v2 + minVertex);
		}

		this->addEdge(v1, v2);
		chunkBeg++;
	}
}


// constructor to build a path of edges through a graph
//Path::Path(int n) {
//	this->pres.resize(n);
//	fill(this->pres.begin(), this->pres.end(), 0);
//}
//
//// set precessor
//void Path::set(int node, int pre) {
//	this->pres[node] = pre;
//}
//
//int* Path::frequency() {
//	//TODO
//}

//// writes a dot graph representation of this graph to stdout
//void Path::printAsDotGraph(void) {
//	printf("graph graphname {\n");
//	for (int i = 1; i < this->vertices.size(); i++) {
//		printf("%d -- %d;\n", this->vertices[i-1], this->vertices[i]);
//	}
//	printf("}\n");
//}
