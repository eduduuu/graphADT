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
void destruct_graph(Graph *g);
void add_vertex(Graph *g, int v);
void add_edge(Graph *g, Edge e);
void add_edges(Graph *g, Edge **e, size_t size); 
void erase_edge(Graph *g, Edge e);
void erase_vertex(Graph *g, int v);
int has_edge(Graph *g, Edge e);
int has_vertex(Graph *g, int v);
Neighborhood neighbors(Graph *g, int v);
Neighborhood next_neighbors(Graph *g, Neighborhood n);
void print_graph(Graph *g);
