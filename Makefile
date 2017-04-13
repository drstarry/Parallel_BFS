bfs: clean
	@mpic++ src/bfs.cpp -o bfs -fopenmp
	@#./bfs
	@command time -p (OMP_NUM_THREADS=16 mpirun -np 8 ./bfs 64 > graph.dot) 2>&1
	#@xdot graph.dot &> /dev/null
	#@rm -f graph.dot

clean:
	@rm -f bfs
