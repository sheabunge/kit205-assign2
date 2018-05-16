#include <stdlib.h>
#include <stdio.h>
#include "graph.h"
#include "dem.h"

/**
 * Display a message and wait for user input
 */
void wait_for_exit() {
	return; // REMOVE BEFORE SUBMISSION
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
	if (diff > 0)
		cost += diff * diff;
	else
		cost += diff;
	return cost;
}

/**
 * Find the most efficient path through a digital elevation map using Dijkstra's algorithm
 * @param dem
 * @param size
 * @return
 */
int **generate_map_A(int **dem, const int size) {

	Graph graph = create_graph(size * size);
	generate_map_graph(dem, size, &graph, cost_funcA);

	int *path = find_shortest_path_A(&graph, 0, size * size - 1);

	int **map = clone_dem(dem, size);
	traverse_map(map, size, path + 1, path[0]);

	destroy_graph(&graph);

	return map;
}

/**
 * Find the most efficient path through a digital elevation map using the Floyd-Warshall algorithm
 * @param dem
 * @param size
 * @return
 */
int **generate_map_B(int **dem, const int size) {

	Graph graph = create_graph(size * size);
	generate_map_graph(dem, size, &graph, cost_funcB);

	int *path = find_shortest_path_A(&graph, 0, size * size - 1);

	int **map = clone_dem(dem, size);
	traverse_map(map, size, path + 1, path[0]);

	destroy_graph(&graph);

	return map;
}


int main() {
	const int SIZE = 5;

	// Randomly generate a digital elevation map
//	int** dem = make_dem(SIZE, SIZE * 4);
	int **dem = static_dem();
	print_2D_ascii(dem, SIZE);

	// Use Dijkstra's algorithm to plot the most efficient path on the map
	printf("\n\n");
	int **map_a = generate_map_A(dem, SIZE);
	print_2D_ascii(map_a, SIZE);

	// Use the Floyd-Warshall algorithm to plot the most efficient path on the map
	printf("\n\n");
	int **map_b = generate_map_B(dem, SIZE);
	print_2D_ascii(map_b, SIZE);

	// Free memory and exit the program
	wait_for_exit();
	destroy_dem(dem, SIZE);
	destroy_dem(map_a, SIZE);
	destroy_dem(map_b, SIZE);
	return 0;
}
