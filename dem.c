#include "dem.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Generate a digital elevation map (DEM)
 *
 * @param size       dimension of map – must be n^2 + 1
 * @param roughness  higher values give rougher terrain
 * @return
 */
int** make_dem(int size, int roughness) {
	unsigned int seed = (unsigned) time(NULL);
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

	int data[size][size] = {
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
 * Print the values in a two-dimensional array
 * @param array2D
 * @param size
 */
void print_2D(int** array2D, int size) {
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			if (array2D[x][y] > 0) {
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
 * Generate a graph from a digital elevation map
 * @param graph  Graph instance to add new edges to
 * @param dem    source digital elevation map
 * @param size   size of map
 */
void generate_map_graph(Graph *graph, int **dem, const int size) {
	const int DIRECTIONS = 4;
	const int VERTICES = size * size;

	// Loop through every point in the map
	for (int src = 0; src < VERTICES; src++) {
		int x1 = src / size;
		int y1 = src % size;

		// Determine points immediately to the south, west, east and north
		const int STEPS[4] = {src + size, src + 1, src - 1, src - size};

		for (int step = 0; step < DIRECTIONS; step++) {
			int dest = STEPS[step];

			int x2 = dest / size;
			int y2 = dest % size;

			// If the point is valid, calculate the cost and add an edge to the graph
			if (0 <= dest && dest < VERTICES) {
				int cost = cost_funcA(dem[x2][y2] - dem[x1][y1]);
				add_edge(graph, src, dest, cost);
			}
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
	int** result = malloc(size * sizeof(int *));

	for (int x = 0; x < size; x++) {
		result[x] = malloc(size * sizeof(int));

		for (int y = 0; y < size; y++) {
			result[x][y] = self[x][y];
		}
	}

	return result;
}

/**
 * Apply a given path to a map
 * @param dem     map to traverse
 * @param size    size of map
 * @param path    path to apply
 * @param length  length of path
 */
void traverse_map(int **dem, const int size, int *path, const int length) {

	for (int i = 0; i < length; i++) {
		int x = path[i] / size;
		int y = path[i] % size;

		dem[x][y] = -1;
	}
}
