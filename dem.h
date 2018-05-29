#pragma once

#include "graph.h"

/**
 * Print the values in a two-dimensional array
 * @param array2D
 * @param size
 */
void print_2D(int **array2D, int size);

/**
 * Print the values in a two-dimensional array as ascii art
 * @param array2D
 * @param size
 */
void print_2D_ascii(int **array2D, int size);

/**
 * Generate a digital elevation map (DEM)
 *
 * @param size       dimension of map – must be n^2 + 1
 * @param roughness  higher values give rougher terrain
 * @return
 */
int **make_dem(int size, int roughness);

/**
 * Copy the contents of a DEM into a new variable
 * @param self
 * @param size
 * @return
 */
int **clone_dem(int **self, int size);

/**
 * Free all memory allocated to a DEM
 * @param self
 * @param size
 */
void destroy_dem(int **self, int size);

/**
 * Generate a graph from a digital elevation map
 * @param self       source digital elevation map
 * @param size       size of map
 * @param graph      Graph instance to add new edges to
 * @param cost_func  function to use for normalising edge weights
 */
void generate_map_graph(int **self, int size, Graph *graph, int cost_func(int));

/**
 * Apply a given path to a map
 * @param self
 * @param size
 * @param path
 */
void traverse_map(int **self, int size, EdgeList path);
