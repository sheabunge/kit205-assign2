#include "dem.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Print the values in a two-dimensional array
 * @param array2D
 * @param size
 */
void print_2D(int** array2D, int size) {
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			if (array2D[x][y] >= 0) {
				printf("%2d ", array2D[x][y]);
			}
			else {
				printf("() ");
			}
		}
		printf("\n");
	}
}

/**
 * Print the values in a two-dimensional array as ascii art
 * @param array2D
 * @param size
 */
void print_2D_ascii(int** array2D, int size) {
	char *shades = " .-:=+*#%@";
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			if (array2D[x][y] >= 0) {
				char shade = shades[array2D[x][y] * 10 / 100];
				printf("%c%c", shade, shade);
			} else {
				printf("()");
			}
		}
		printf("\n");
	}
}

/**
 * Generate a digital elevation map (DEM)
 *
 * @param size       dimension of map – must be n^2 + 1
 * @param roughness  higher values give rougher terrain
 * @return
 */
int** make_dem(int size, int roughness) {
	unsigned seed = 0; // (unsigned) time(NULL);
	srand(seed);
	int** dem = malloc(size * sizeof *dem);
	for (int x = 0; x < size; x++) {
		dem[x] = malloc(size * sizeof *dem[x]);
		for (int y = 0; y < size; y++) {
			dem[x][y] = -1;
		}
	}
	int r = roughness;

	dem[0][0] =				50 - r / 2 + rand() % r;
	dem[size-1][0] =		50 - r / 2 + rand() % r;
	dem[0][size-1] =		50 - r / 2 + rand() % r;
	dem[size-1][size-1] =	50 - r / 2 + rand() % r;

	for (int step = (size - 1); step > 0; step /= 2) {
		r = r > 1 ? r / 2:r;
		if (r < 1) r = 1;
		for (int cx = 0; cx < (size-1) / step; cx++) {
			for (int cy = 0; cy < (size-1) / step; cy++) {
				int a = dem[cx*step][cy*step];
				int b = dem[cx*step+step][cy*step];
				int c = dem[cx*step][cy*step+step];
				int d = dem[cx*step+step][cy*step+step];

				dem[cx*step + step / 2][cy*step + step / 2] = (a + b + c + d) / 4 + rand() % r - r/2;

				dem[cx*step + step / 2][cy*step]		= (a + b) / 2 + rand() % r - r/2;
				dem[cx*step][cy*step + step / 2]		= (a + c) / 2 + rand() % r - r/2;
				dem[cx*step + step][cy*step + step / 2] = (b + d) / 2 + rand() % r-r/2;
				dem[cx*step + step/2][cy*step + step]	= (c + d) / 2 + rand() % r-r/2;
			}
		}
	}
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			dem[x][y] = dem[x][y]<0	? 0 : dem[x][y];
			dem[x][y] = dem[x][y]>99? 99: dem[x][y];
		}
	}
	return dem;
}

int **static_dem() {
	const int size = 5;

	/*int data[size][size] = {
			{49, 51, 51, 56, 57},
			{46, 47, 50, 52, 54},
			{45, 47, 48, 50, 49},
			{46, 46, 47, 47, 47},
			{45, 45, 45, 45, 47}
	};*/

	int data[5][5] = {
			{12, 14, 15, 15, 16},
			{16, 18, 18, 19, 17},
			{18, 19, 21, 20, 17},
			{19, 20, 18, 18, 15},
			{20, 17, 14, 14, 13}
	};


	int** dem = malloc(size * sizeof *dem);

	for (int x = 0; x < size; x++) {
		dem[x] = malloc(size * sizeof *dem[x]);

		for (int y = 0; y < size; y++) {
			dem[x][y] = data[x][y];
		}
	}

	return dem;
}

/**
 * Generate a graph from a digital elevation map
 * @param self       source digital elevation map
 * @param size       size of map
 * @param graph      Graph instance to add new edges to
 * @param cost_func  function to use for normalising edge weights
 */
void generate_map_graph(int **self, const int size, Graph *graph, int cost_func(int)) {
	const int VERTICES = size * size;

	// Loop through every point in the map
	for (int start = 0; start < VERTICES; start++) {
		int x1 = start / size;
		int y1 = start % size;

		// Create a list of potential movement destinations
		int destinations[4];
		int moves = 0;

		// Can move south if not at the bottom of the map
		if (x1 != size - 1) {
			destinations[moves] = start + size;
			moves++;
		}

		// Can move west if not at the left edge of the map
		if (y1 != 0) {
			destinations[moves] = start - 1;
			moves++;
		}

		// Can move east if not at the right edge of the map
		if (y1 != size - 1) {
			destinations[moves] = start + 1;
			moves++;
		}

		// Can move north if not at the top of the map
		if (x1 != 0) {
			destinations[moves] = start - size;
			moves++;
		}

		for (int i = 0; i < moves; i++) {
			int dest = destinations[i];

			int x2 = dest / size;
			int y2 = dest % size;

			int cost = cost_func(self[x2][y2] - self[x1][y1]);
			add_edge(graph, start, dest, cost);
		}
	}
}

/**
 * Copy the contents of a DEM into a new variable
 * @param self
 * @param size
 * @return
 */
int **clone_dem(int **self, const int size) {
	int** result = malloc(size * sizeof *result);

	for (int x = 0; x < size; x++) {
		result[x] = malloc(size * sizeof result[x]);

		for (int y = 0; y < size; y++) {
			result[x][y] = self[x][y];
		}
	}

	return result;
}

/**
 * Free all memory allocated to a DEM
 * @param self
 * @param size
 */
void destroy_dem(int **self, const int size) {

	for (int x = 0; x < size; x++) {
		free(self[x]);
	}

	free(self);
}

/**
 * Apply a given path to a map
 * @param self
 * @param size
 * @param path
 */
void traverse_map(int **self, const int size, EdgeList path) {
	EdgeNodePtr current = path.head;

	while (current != NULL) {
		int vertex = current->edge.to_vertex;

		int x = vertex / size;
		int y = vertex % size;

		self[x][y] = -1;

		current = current->next;
	}
}
