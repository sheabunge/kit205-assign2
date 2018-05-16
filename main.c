#include <stdlib.h>
#include <stdio.h>
#include "graph.h"
#include "dem.h"


/**
 * Main function
 * @return
 */
int main() {
	const int SIZE = 5;

//	int** dem = make_dem(SIZE, SIZE * 4);
	int **dem = static_dem();

	Graph graph = create_graph(SIZE * SIZE);
	generate_map_graph(&graph, dem, SIZE);

	int last_vertex = SIZE * SIZE - 1;

	int *path = find_shortest_path(&graph, 0, last_vertex);

	for (int i = 1; i <= path[0]; i++) {
		printf("%d", path[i]);
		if (i != path[0]) {
			printf(" → ");
		}
	}

//	printf("\npress enter to exit\n");
//	getchar();

	destroy_graph(&graph);

	return 0;
}
