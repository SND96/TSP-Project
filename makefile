CC=g++

sa.o: simulated_annealing.cpp
	$(CC) -c -o $@ $^

samake: sa.o
	$(CC) -o sa.out $^ -lstdc++fs

clean:
	rm sa.o sa.out

