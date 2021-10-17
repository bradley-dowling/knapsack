knapsack: main.o
	gcc -o knapsack main.o
main.o: main.c structures.h
	gcc -c main.c
clean:
	rm knapsack main.o
