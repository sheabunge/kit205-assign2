#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include "graph.h"

/**
 * Generate a digital elevation map (DEM)
 *
 * @param size       dimension of map – must be n^2 + 1
 * @param roughness  higher values give rougher terrain
 * @return
 */
int** make_dem(int size, int roughness) {
	int seed = time(NULL);
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
 * Run Dijkstra's algorithm on a graph
 * @param graph   instance of Graph
 * @param source  vertex number to begin paths at
 * @param dist    empty array of length graph->V for storing shortest path distances
 * @param prev    empty array of length graph->V for storing the previous vertex in the paths
 */
void dijkstra(Graph *graph, int source, int *dist, int *prev) {

	// Instead of using sets, use an array to determine if each node has been visited
	int *unvisited = malloc(sizeof(int) * graph->V);

	// Also keep a running count of how many nodes have been visited
	int visits = 0;

	// Initialise the arrays with default values
	for (int v = 0; v < graph->V; v++) {
		dist[v] = -1;
		prev[v] = -1;
		unvisited[v] = true;
	}

	// The source vertex is zero distance from itself
	dist[source] = 0;

	// Continue looping until all vertices have been visited
	while (visits < graph->V) {

		// Find the unvisited vertex with the shortest recorded distance
		int u = -1;

		for (int v = 0; v < graph->V; v++) {
			if (unvisited[v] && (u == -1 || dist[v] < dist[u])) {
				u = v;
			}
		}

		// Mark this vertex as visited
		unvisited[u] = false;
		visits++;

		// Loop through all outgoing edges of this vertex
		EdgeNodePtr current = graph->edges[u].head;
		while (current != NULL) {
			int alt = dist[u] + current->edge.weight;
			int dest = current->edge.to_vertex;

			if (unvisited[dest] && (dist[dest] == -1 || alt < dist[dest])) {
				dist[dest] = alt;
				prev[dest] = u;
			}

			current = current->next;
		}
	}

	free(unvisited);
}

int *find_shortest_path(Graph *graph, int source, int target, int *length) {

	/* run Dijkstra's algorithm to retrieve a list of shortest paths */
	int *dist = malloc(sizeof(int) * graph->V);
	int *prev = malloc(sizeof(int) * graph->V);
	dijkstra(graph, source, dist, prev);

	int *path = malloc(sizeof(int) * dist[target]);

	int i = 0;
	int vertex = target;

	while (prev[vertex] != -1) {
		printf("%d ", prev[vertex]);
		path[i] = prev[vertex];
		vertex = prev[vertex];
		i++;
	}

	path[i] = prev[vertex];
	*length = i;

	return path;
}

int main() {
	const int SIZE = 5;

//	int** dem = make_dem(SIZE, SIZE * 4);
	int **dem = static_dem();

	Graph graph = create_graph(SIZE * SIZE);
	generate_map_graph(&graph, dem, SIZE);

	for (int v = 0; v < graph.V; v++) {
		EdgeNodePtr current = graph.edges[v].head;

		while (current != NULL) {
			printf("%d -> %d (%d)\n", v, current->edge.to_vertex, current->edge.weight);
			current = current->next;

			if (current == NULL) {
				printf("\n");
			}
		}
	}

	print_2D(dem, SIZE);

	printf("\npress enter to exit\n");
	getchar();

	destroy_graph(&graph);

	return 0;
}
