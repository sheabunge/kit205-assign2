#include <stdlib.h>
#include <stdio.h>
#include "graph.h"
#include "dem.h"

void wait_for_exit() {
	return; // REMOVE BEFORE SUBMISSION
	printf("\npress enter to exit\n");
	getchar();
}

void mission_A(int **dem, const int size) {

	Graph graph = create_graph(size * size);
	generate_map_graph(dem, size, &graph);

	int *path = find_shortest_path(&graph, 0, size * size - 1);

	int **map = clone_dem(dem, size);
	traverse_map(map, size, path + 1, path[0]);

	print_2D_ascii(map, size);

	wait_for_exit();
	destroy_graph(&graph);
}


int main() {
	const int SIZE = 5;

//	int** dem = make_dem(SIZE, SIZE * 4);
	int **dem = static_dem();

	print_2D_ascii(dem, SIZE);
	printf("\n\n");

	mission_A(dem, SIZE);

	destroy_dem(dem, SIZE);
	return 0;
}
