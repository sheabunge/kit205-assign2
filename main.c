#include <stdlib.h>
#include <stdio.h>
#include "graph.h"
#include "dem.h"

void wait_for_exit() {
	return; // REMOVE BEFORE SUBMISSION
	printf("\npress enter to exit\n");
	getchar();
}

int **generate_map_A(int **dem, const int size) {

	Graph graph = create_graph(size * size);
	generate_map_graph(dem, size, &graph);

	int *path = find_shortest_path(&graph, 0, size * size - 1);

	int **map = clone_dem(dem, size);
	traverse_map(map, size, path + 1, path[0]);

	destroy_graph(&graph);

	return map;
}


int main() {
	const int SIZE = 5;

//	int** dem = make_dem(SIZE, SIZE * 4);
	int **dem = static_dem();

	print_2D_ascii(dem, SIZE);
	printf("\n\n");

	int **map_a = generate_map_A(dem, SIZE);
	print_2D_ascii(map_a, SIZE);

	wait_for_exit();
	destroy_dem(dem, SIZE);
	destroy_dem(map_a, SIZE);
	return 0;
}
