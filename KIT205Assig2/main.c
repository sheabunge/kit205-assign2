#include <stdlib.h>
#include <stdio.h>
#include "graph.h"
#include "dem.h"

/**
 * Display a message and wait for user input
 */
void wait_for_exit() {
	printf("\npress enter to exit\n");
	getchar();
}

/**
 * Returns the cost of moving between adjacent squares and increasing altitude
 * @param diff height difference between squares
 * @return movement cost
 */
int cost_funcA(int diff) {
	int cost = 1;
	if (diff > 0) cost += diff * diff;
	return cost;
}

/**
 * Returns the cost of moving between adjacent squares and increasing altitude
 * @param diff height difference between squares
 * @return movement cost
 */
int cost_funcB(int diff) {
	int cost = 1;
	if (diff > 0) {
		cost += diff * diff;
	} else {
		cost += diff;
	}
	return cost;
}

/**
 * Find the most efficient path through a digital elevation map
 * @param dem
 * @param size
 * @param find_shortest_path
 * @param cost_func
 * @return
 */
void run_mission(int **dem, int size, EdgeList find_shortest_path(Graph *, int, int, int *), int cost_func(int)) {

	// Create a graph and populate it from the digital elevation map
	Graph graph = create_graph(size * size);
	generate_map_graph(dem, size, &graph, cost_func);

	// Use the provided shortest path algorithm to find the most efficient path through the map
	int energy;
	EdgeList path = find_shortest_path(&graph, 0, size * size - 1, &energy);

	// Clone the provided map and plot the path onto it
	int **map = clone_dem(dem, size);
	traverse_map(map, size, path);

	print_2D_ascii(map, size);
	printf("\ntotal energy: %d\n", energy);

	// Free memory allocated to data structures
	destroy_dem(map, size);
	destroy_graph(&graph);
}

int main() {
	const int SIZE = 33;

	// Randomly generate a digital elevation map
	int **dem = make_dem(SIZE, SIZE * 4);
	print_2D_ascii(dem, SIZE);
	printf("\n\n");

	// Use Dijkstra's algorithm to plot the most efficient path on the map
	run_mission(dem, SIZE, find_shortest_path_A, cost_funcA);
	printf("\n\n");

	// Use the Floyd-Warshall algorithm to plot the most efficient path on the map
	run_mission(dem, SIZE, find_shortest_path_B, cost_funcB);

	wait_for_exit();
	destroy_dem(dem, SIZE);
	return 0;
}
