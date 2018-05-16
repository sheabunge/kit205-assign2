#pragma once

typedef struct edge {
	int to_vertex;
	int weight;
} Edge;

typedef struct edgeNode {
	Edge edge;
	struct edgeNode *next;
} *EdgeNodePtr;

typedef struct edgeList {
	EdgeNodePtr head;
} EdgeList;

typedef struct graph {
	int V;
	EdgeList *edges;
} Graph;


/**
 * Create a new instance of the graph type
 * @param vertices  amount of vertices in the graph
 * @return newly-created Graph instance
 */
Graph create_graph(int vertices);

/**
 * Add a new directed edge to a graph
 * @param self    Graph instance
 * @param from    vertex where the edge begins
 * @param dest    vertex where the edge ends
 * @param weight  edge weight
 */
void add_edge(Graph *self, int from, int dest, int weight);

/**
 * Retrieve the shortest path from a given source to a destination
 *
 * @param self    Graph instance
 * @param source  vertex where path should begin
 * @param target  vertex where path should end
 *
 * @return array with index 0 set to the length of the path, followed by ordered path vertices
 */
int *find_shortest_path(Graph *self, int source, int target);

/**
 * Free all memory used by a graph instance
 * @param self  Graph instance
 */
void destroy_graph(Graph *self);
