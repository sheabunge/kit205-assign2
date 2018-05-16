#pragma once
#include "graph.h"

/**
 * Generate a digital elevation map (DEM)
 *
 * @param size       dimension of map – must be n^2 + 1
 * @param roughness  higher values give rougher terrain
 * @return
 */
int** make_dem(int size, int roughness);

int **static_dem();

/**
 * Print the values in a two-dimensional array
 * @param array2D
 * @param size
 */
void print_2D(int** array2D, int size);

/**
 * Print the values in a two-dimensional array as ascii art
 * @param array2D
 * @param size
 */
void print_2D_ascii(int** array2D, int size);

/**
 * Generate a graph from a digital elevation map
 * @param graph  Graph instance to add new edges to
 * @param dem    source digital elevation map
 * @param size   size of map
 */
void generate_map_graph(Graph *graph, int **dem, int size);
