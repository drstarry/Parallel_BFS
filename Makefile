all: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp # the warnings are freaking awful

quiet: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp 2>/dev/null

run: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp 2>/dev/null
	@OMP_NUM_THREADS=4 mpirun -np 4 ./centrality 64

run_single: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp 2>/dev/null
	@OMP_NUM_THREADS=4 mpirun -np 4 ./centrality 64

run_with_graph: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp 2>/dev/null
	@OMP_NUM_THREADS=16 mpirun -np 8 ./centrality 1024
	@echo "rendering dot graph..."
	@sfdp -Goverlap_scaling=-10 -Goverlap=prism graph-1.dot -x -Tsvg -o graph-1.svg 2>/dev/null

run_with_graph_xdot: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp 2>/dev/null
	@OMP_NUM_THREADS=16 mpirun -np 8 ./centrality 128
	@echo "rendering dot graph..."
	@xdot graph-1.dot 2>/dev/null

clean:
	@rm -f centrality graph-*.dot graph-*.svg centrality-*.txt
