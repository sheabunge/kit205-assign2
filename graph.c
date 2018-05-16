#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include "graph.h"

#define INFINITY INT_MAX

/**
 * Create a new instance of the graph type
 * @param vertices  amount of vertices in the graph
 * @return newly-created Graph instance
 */
Graph create_graph(int vertices) {
	Graph self;

	self.V = vertices;
	self.edges = malloc(sizeof(self.edges) * self.V);

	for (int v = 0; v < self.V; v++) {
		self.edges[v].head = NULL;
	}

	return self;
}

void check_vertex(Graph *self, int vertex) {
	if (vertex < 0 || vertex >= self->V) {
		fprintf(stderr, "vertex %d is out of range", vertex);
	}
}

/**
 * Add a new directed edge to a graph
 * @param self    Graph instance
 * @param from    vertex where the edge begins
 * @param dest    vertex where the edge ends
 * @param weight  edge weight
 */
void add_edge(Graph *self, int from, int dest, int weight) {
	EdgeNodePtr node = malloc(sizeof(node));

	check_vertex(self, from);
	check_vertex(self, dest);

	node->edge.to_vertex = dest;
	node->edge.weight = weight;

	node->next = self->edges[from].head;
	self->edges[from].head = node;
}

/**
 * Free all memory used by a graph instance
 * @param self  Graph instance
 */
void destroy_graph(Graph *self) {

	for (int v = 0; v < self->V; v++) {
		EdgeNodePtr current = self->edges[v].head;

		while (current != NULL) {
			EdgeNodePtr to_free = current;
			current = current->next;
			free(to_free);
		}
	}

	free(self->edges);
}

/**
 * Run Dijkstra's algorithm on a graph
 * @param self    Graph instance
 * @param source  vertex number to search from
 * @param dist    empty array of length self->V for storing shortest path distances
 * @param prev    empty array of length self->V for storing the previous vertex in the paths
 */
void dijkstra(Graph *self, int source, int *dist, int *prev) {

	// Instead of using sets, use an array to determine if each node has been visited
	int *unvisited = malloc(sizeof(bool) * self->V);

	// Also keep a running count of how many nodes have been visited
	int visits = 0;

	// Initialise the arrays with default values
	for (int v = 0; v < self->V; v++) {
		dist[v] = INFINITY;
		prev[v] = -1;
		unvisited[v] = true;
	}

	// The source vertex is zero distance from itself
	dist[source] = 0;

	// Continue looping until all vertices have been visited
	while (visits < self->V) {

		// Find the unvisited vertex with the shortest recorded distance
		int u = -1;

		for (int v = 0; v < self->V; v++) {
			if (unvisited[v] && (u == -1 || dist[v] < dist[u])) {
				u = v;
			}
		}

		// Mark this vertex as visited
		unvisited[u] = false;
		visits++;

		// Loop through all outgoing edges of this vertex
		EdgeNodePtr current = self->edges[u].head;
		while (current != NULL) {
			int alt = dist[u] + current->edge.weight;
			int dest = current->edge.to_vertex;

			if (unvisited[dest] && alt < dist[dest]) {
				dist[dest] = alt;
				prev[dest] = u;
			}

			current = current->next;
		}
	}

	free(unvisited);
}

/**
 * Retrieve the shortest path from a given source to a destination
 *
 * @param self    Graph instance
 * @param source  vertex where path should begin
 * @param target  vertex where path should end
 *
 * @return array with index 0 set to the length of the path, followed by ordered path vertices
 */
int *find_shortest_path_A(Graph *self, int source, int target) {

	/* run Dijkstra's algorithm to retrieve a list of shortest paths */
	int *dist = malloc(sizeof(int) * self->V);
	int *prev = malloc(sizeof(int) * self->V);
	dijkstra(self, source, dist, prev);

	// allocate memory for the largest possible path
	int *path = malloc(sizeof(int) * self->V);

	// trace the path from the target back to the source
	int length = 0;
	int vertex = target;

	while (vertex != -1) {
		path[length] = vertex;
		vertex = prev[vertex];
		length++;
	}

	// create a more appropriately sized array for the final path
	int *final = malloc(sizeof(int) * (length + 1));
	final[0] = length; // set the first index to the path length

	// loop through the vertices in the path and add them to the final array in the correct order
	for (int i = 0; i < length; i++) {
		final[i + 1] = path[length - 1 - i];
	}

	// remove the temporary array and return the result
	free(path);
	return final;
}

void floyd(Graph *self, int** dist, int**next) {

	// initialise the two-dimensional arrays with default values
	for (int i = 0; i < self->V; i++) {
		for (int j = 0; j < self->V; j++) {
			dist[i][j] = INFINITY;
			next[i][j] = -1;
		}
	}

	// traverse the graph and add weights to the array
	for (int src = 0; src < self->V; src++) {
		EdgeNodePtr current = self->edges[src].head;

		while (current != NULL) {
			int dest = current->edge.to_vertex;

			dist[src][dest] = current->edge.weight;

			current = current->next;
		}

		// set the distance from the vertex to itself to zero
		dist[src][src] = 0;
	}

	for (int k = 1; k < self->V; k++) {
		for (int i = 1; i < self->V; i++) {
			for (int j = 1; j < self->V; j++) {

				if (dist[i][j] > dist[i][k] + dist[k][j]) {
					dist[i][j] = dist[i][k] + dist[k][j];
					next[i][j] = next[i][k];
				}
			}
		}
	}
}

/**
 * Retrieve the shortest path from a given source to a destination
 *
 * @param self    Graph instance
 * @param source  vertex where path should begin
 * @param target  vertex where path should end
 *
 * @return array with index 0 set to the length of the path, followed by ordered path vertices
 */
int *find_shortest_path_B(Graph *self, int source, int target) {

	// Allocate memory for the two-dimensional arrays
	int **dist = malloc(sizeof(int *) * self->V);
	int **next = malloc(sizeof(int *) * self->V);

	for (int i = 0; i < self->V; i++) {
		dist[i] = malloc(sizeof(int) * self->V);
		next[i] = malloc(sizeof(int) * self->V);
	}

	// Run the Floyd-Warshall algorithm
	floyd(self, dist, next);

	// Retrace the path from the source vertex to the target
	int *path = malloc(sizeof(int) * self->V);
	int length = 0;

	int vertex = source;
	path[0] = source;

	while (vertex != target) {
		vertex = next[vertex][target];
		path[length] = vertex;
		length++;
	}

	// Transpose the path into a more appropriately-sized array
	int *final = malloc(sizeof(int) * (length + 1));
	final[0] = length;

	for (int i = 0; i < length; i++) {
		final[i + 1] = path[i];
	}

	// Free all allocated memory
	for (int i = 0; i < self->V; i++) {
		free(dist[i]);
		free(next[i]);
	}

	free(path);
	free(dist);
	free(next);

	return final;
}
