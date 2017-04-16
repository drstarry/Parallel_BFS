bfs: clean
	@mpic++ src/bfs.cpp src/graph.cpp -o bfs -fopenmp # the warning are freaking awful
	@#command time -p OMP_NUM_THREADS=16 mpirun -np 8 ./bfs 10 > graph.dot 2>&1
	@./bfs 10 > graph.dot
	@xdot graph.dot &> /dev/null

clean:
	@rm -f bfs graph.dot
