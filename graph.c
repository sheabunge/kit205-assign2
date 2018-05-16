#include <stdlib.h>
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
