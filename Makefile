all: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp # the warning are freaking awful
	@#command time -p OMP_NUM_THREADS=16 mpirun -np 8 ./centrality 64 2>&1
	@#./centrality 64
	@#xdot graph.dot &> /dev/null
	@#./centrality 40

quiet: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp &>/dev/null

run: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp &>/dev/null
	@OMP_NUM_THREADS=4 mpirun -np 4 ./centrality 64

run_single: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp &>/dev/null
	@OMP_NUM_THREADS=1 mpirun -np 1 ./centrality 64

run_with_graph: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp &>/dev/null
	@OMP_NUM_THREADS=1 mpirun -np 1 ./centrality 16
	@xdot graph-1.dot &> /dev/null

clean:
	@rm -f centrality graph-*.dot
