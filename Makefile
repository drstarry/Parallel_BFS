centrality: clean
	@mpic++ src/centrality.cpp src/graph.cpp src/utils.cpp -o centrality -fopenmp # the warning are freaking awful
	@#command time -p OMP_NUM_THREADS=16 mpirun -np 8 ./centrality 10 > graph.dot 2>&1
	@./centrality 10 > graph.dot
	@xdot graph.dot &> /dev/null

clean:
	@rm -f centrality graph.dot
