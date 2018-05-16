#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "graph.h"

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

/**
 * Add a new directed edge to a graph
 * @param self    Graph instance
 * @param from    vertex where the edge begins
 * @param dest    vertex where the edge ends
 * @param weight  edge weight
 */
void add_edge(Graph *self, int from, int dest, int weight) {
	EdgeNodePtr node = malloc(sizeof(EdgeNodePtr));

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
 * @param graph   instance of Graph
 * @param source  vertex number to search from
 * @param dist    empty array of length graph->V for storing shortest path distances
 * @param prev    empty array of length graph->V for storing the previous vertex in the paths
 */
void dijkstra(Graph *graph, int source, int *dist, int *prev) {

	// Instead of using sets, use an array to determine if each node has been visited
	int *unvisited = malloc(sizeof(bool) * graph->V);

	// Also keep a running count of how many nodes have been visited
	int visits = 0;

	// Initialise the arrays with default values
	for (int v = 0; v < graph->V; v++) {
		dist[v] = INT_MAX;
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
int *find_shortest_path(Graph *self, int source, int target) {

	/* run Dijkstra's algorithm to retrieve a list of shortest paths */
	int *dist = malloc(sizeof(int) * self->V);
	int *prev = malloc(sizeof(int) * self->V);
	dijkstra(self, source, dist, prev);

	// allocate memory for the largest possible path
	int *path = malloc(sizeof(int) * self->V);

	// trace the path from the target back to the source
	int length = 0;
	int vertex = target;

	// continue looping until the previous vertex is undefined (source vertex reached)
	while (vertex != -1) {
		path[length] = vertex;
		vertex = prev[vertex];
		length++;
	}

	// create an array more appropriately sized for the final path
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
