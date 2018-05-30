#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "graph.h"

// using INT_MAX breaks the Floyd-Warshall algorithm; this is a safer value
const int INFINITY = INT_MAX / 2;

/**
 * Create a new instance of the graph type
 * @param vertices  amount of vertices in the graph
 * @return newly-created Graph instance
 */
Graph create_graph(int vertices) {
	Graph self;

	self.V = vertices;
	self.edges = malloc(self.V * sizeof *self.edges);

	for (int v = 0; v < self.V; v++) {
		self.edges[v].head = NULL;
	}

	return self;
}

/**
 * Ensure that a specific vertex exists on a graph
 * @param self
 * @param vertex
 */
void check_vertex(Graph *self, int vertex) {

	if (vertex < 0 || vertex >= self->V) {
		fprintf(stderr, "vertex %d is out of range\n", vertex);
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

	// ensure both vertices are valid
	check_vertex(self, from);
	check_vertex(self, dest);

	EdgeNodePtr node = malloc(sizeof *node);

	node->edge.to_vertex = dest;
	node->edge.weight = weight;

	// insert the new edge node at the beginning of the existing list
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
	bool *unvisited = malloc(self->V * sizeof *unvisited);

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
 * Retrieve the shortest path from a given source to a destination using Dijkstra's algorithm
 *
 * @param self      Graph instance
 * @param source    vertex where the path should begin
 * @param target    vertex where the path should end
 * @param distance  pointer to address for storing total distance used by the path
 *
 * @return linked list of path vertices
 */
EdgeList find_shortest_path_A(Graph *self, int source, int target, int *distance) {

	// run Dijkstra's algorithm to retrieve a list of shortest paths
	int *dist = malloc(self->V * sizeof *dist);
	int *prev = malloc(self->V * sizeof *prev);
	dijkstra(self, source, dist, prev);

	// pass the total path distance out of the function through a variable pointer
	if (distance != NULL) {
		*distance = dist[target];
	}

	// trace the path from the target back to the source
	EdgeList path;
	path.head = NULL;

	int vertex = target;

	// the source vertex has -1 as its previous value
	while (vertex != -1) {
		// construct a new edge node leading to the current vertex
		EdgeNodePtr node = malloc(sizeof *node);
		node->edge.to_vertex = vertex;
		node->edge.weight = 0;

		// insert the new node at the beginning of the current path
		node->next = path.head;
		path.head = node;

		vertex = prev[vertex];
	}

	free(dist);
	free(prev);

	return path;
}

/**
 * Run the Floyd–Warshall algorithm on a graph
 * @param self  Graph instance
 * @param dist  Pointer to empty two-dimensional array of size self->V square
 * @param next  Pointer to empty two-dimensional array of size self->V square
 */
void floyd(Graph *self, int **dist, int **next) {

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
			next[src][dest] = dest;
			current = current->next;
		}

		// set the distance from the vertex to itself to zero
		dist[src][src] = 0;
	}

	for (int k = 0; k < self->V; k++) {
		for (int i = 0; i < self->V; i++) {
			for (int j = 0; j < self->V; j++) {

				if (dist[i][j] > dist[i][k] + dist[k][j]) {
					dist[i][j] = dist[i][k] + dist[k][j];
					next[i][j] = next[i][k];
				}
			}
		}
	}
}

/**
 * Retrieve the shortest path from a given source to a destination using the Floyd-Warshall algorithm
 *
 * @param self      Graph instance
 * @param source    vertex where the path should begin
 * @param target    vertex where the path should end
 * @param distance  pointer to address for storing total distance used by the path
 *
 * @return linked list of path vertices
 */
EdgeList find_shortest_path_B(Graph *self, int source, int target, int *distance) {

	// allocate memory for the two-dimensional arrays
	int **dist = malloc(self->V * sizeof *dist);
	int **next = malloc(self->V * sizeof *next);

	for (int i = 0; i < self->V; i++) {
		dist[i] = malloc(self->V * sizeof *dist[i]);
		next[i] = malloc(self->V * sizeof *next[i]);
	}

	// run the Floyd-Warshall algorithm
	floyd(self, dist, next);

	// pass the total path distance out of the function through a variable pointer
	if (distance != NULL) {
		*distance = dist[source][target];
	}

	// create a new linked list to hold the path
	EdgeList path;
	path.head = NULL;

	// only attempt to retrace the path if a valid one exists
	if (next[source][target] != -1) {

		// retrace the path from the source vertex to the target
		int vertex = source;
		EdgeNodePtr prev = NULL;

		while (vertex != target) {

			// create a new node to insert into the list
			EdgeNodePtr node = malloc(sizeof *node);
			node->next = NULL;
			node->edge.to_vertex = vertex;
			node->edge.weight = 0;

			// insert the new node immediately after the previous one
			if (prev == NULL) {
				path.head = node;
			} else {
				prev->next = node;
			}

			prev = node;
			vertex = next[vertex][target];
		}
	}

	// Free all allocated memory
	for (int i = 0; i < self->V; i++) {
		free(dist[i]);
		free(next[i]);
	}

	free(dist);
	free(next);

	return path;
}
