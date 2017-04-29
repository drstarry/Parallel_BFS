all: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp # the warnings are freaking awful

quiet: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp &>/dev/null

run: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp &>/dev/null
	@OMP_NUM_THREADS=4 mpirun -np 4 ./centrality 64

run_single: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp &>/dev/null
	@OMP_NUM_THREADS=4 mpirun -np 4 ./centrality 64

run_with_graph: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp &>/dev/null
	@OMP_NUM_THREADS=1 mpirun -np 4 ./centrality 128
	@#xdot graph-1.dot &> /dev/null &
	@#xdot graph-2.dot &> /dev/null &
	@#xdot graph-3.dot &> /dev/null &
	@#xdot graph-4.dot &> /dev/null
	@xdot graph-1.dot &> /dev/null

clean:
	@rm -f centrality graph-*.dot
