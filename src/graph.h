#ifndef GRAPH_H
#define GRAPH_H
#include <stdlib.h>
typedef struct Graph Graph;

typedef struct Edge {
  int u, v;
} Edge;

typedef struct Neighborhood {
  int const * neighbors;
  size_t blockSize;
} Neighborhood;

Graph *create_graph(int v);
int get_num_vertices(Graph *g);
void destruct_graph(Graph *g);
void add_vertex(Graph *g, int v);
void add_edge(Graph *g, Edge e);
void add_edges(Graph *g, Edge *e, size_t size); 
void erase_edge(Graph *g, Edge e);
void erase_vertex(Graph *g, int v);
int has_edge(Graph *g, Edge e);
int has_vertex(Graph *g, int v);
Neighborhood neighbors(Graph *g, int v);
Neighborhood next_neighbors(Graph *g, Neighborhood n);
void destruct_neighborhood(Neighborhood n);
Graph *copy_graph(Graph *g);
void print_graph(Graph *g);

Graph *generate_graph(int v, double density, double shuffle_factor);

int assert_vertex(int (*p_func)(Graph*, int), Graph *p_g, int v, int expectedValue);
int assert_edge(int (*p_func)(Graph*, Edge), Graph *p_g, Edge e, int expectedValue);
#endif
