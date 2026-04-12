#include <stdio.h>
#include "graph.h"

struct Graph {
  int num_vertices;
  int *adjacencyMatrix;
};

Graph
*create_graph(int v) {
  Graph *g = (Graph*)malloc(sizeof(Graph));
  g->num_vertices = v;
  g->adjacencyMatrix = (int*)malloc(sizeof(int) * v * v);
  for (int i = 0; i < v * v; i++)
    g->adjacencyMatrix[i] = 0;

  return g;
}

void destruct_graph(Graph *g) {
  free(g->adjacencyMatrix);
  free(g);
}
void add_vertices(Graph *g, int v);

void
add_edge(Graph *g, Edge e) {
  g->adjacencyMatrix[e.u*g->num_vertices + e.v] = 1;
}

void
add_edges(Graph *g, Edge **e, size_t size) {
  for (size_t i = 0; i < size; i++)
    add_edge(g, *(e[i]));
}
void
erase_edge(Graph *g, Edge e) {
  g->adjacencyMatrix[e.u*g->num_vertices + e.v] = 0;
}
void erase_vertex(Graph *g, int v);
void has_edge(Graph *g, Edge *e);

Neighborhood
neighbors(Graph *g, int v) {
  Neighborhood n = {(const int *)&g->adjacencyMatrix[v], v};
  return n;
}

Neighborhood
next_neighbors(Graph *g, Neighborhood n) {
  return (Neighborhood){NULL, 0};
}

void
print_graph(Graph *g) {
  for (int i = 0; i < g->num_vertices; i++) {
    printf("\n%d ", i);
    for (int j = 0; j < g->num_vertices; j++)
      if (g->adjacencyMatrix[i*g->num_vertices + j])
        printf("%d ", j);
  }
}
