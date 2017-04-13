bfs: clean
	@mpic++ src/bfs.cpp src/graph.cpp -o bfs -fopenmp # the warning are freaking awful
	@#command time -p OMP_NUM_THREADS=16 mpirun -np 8 ./bfs 64 > graph.dot 2>&1
	@./bfs 64 > graph.dot
	@xdot graph.dot &> /dev/null
	@rm -f graph.dot

clean:
	@rm -f bfs
