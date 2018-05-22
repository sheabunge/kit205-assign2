#pragma once

/**
 * Represents an edge in a graph
 */
typedef struct edge {
	int to_vertex;
	int weight;
} Edge;

/**
 * Represents a linked list node containing a graph edge
 */
typedef struct edgeNode {
	Edge edge;
	struct edgeNode *next;
} *EdgeNodePtr;

/**
 * Represents a linked list of edges
 */
typedef struct edgeList {
	EdgeNodePtr head;
} EdgeList;

/**
 * Represents a graph with vertices and edges
 */
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
 * Free all memory used by a graph instance
 * @param self  Graph instance
 */
void destroy_graph(Graph *self);

/**
 * Add a new directed edge to a graph
 * @param self    Graph instance
 * @param from    vertex where the edge begins
 * @param dest    vertex where the edge ends
 * @param weight  edge weight
 */
void add_edge(Graph *self, int from, int dest, int weight);

/**
 * Retrieve the shortest path from a given source to a destination using Dijkstra's algorithm
 *
 * @param self      Graph instance
 * @param source    vertex where path should begin
 * @param target    vertex where path should end
 * @param distance  pointer to address for storing total distance used by the path
 *
 * @return linked list of path vertices
 */
EdgeList find_shortest_path_A(Graph *self, int source, int target, int *distance);

/**
 * Retrieve the shortest path from a given source to a destination using the Floyd-Warshall algorithm
 *
 * @param self      Graph instance
 * @param source    vertex where path should begin
 * @param target    vertex where path should end
 * @param distance  pointer to address for storing total distance used by the path
 *
 * @return linked list of path vertices
 */
EdgeList find_shortest_path_B(Graph *self, int source, int target, int *distance);
