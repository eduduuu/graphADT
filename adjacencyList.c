#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

typedef struct Vertex {
  int v;
  struct Vertex *n;
} Vertex;

struct Graph {
  int num_vertices;
  Vertex **adjacencyList;
};

Graph*
create_graph(int v) {
  Graph *g = (Graph*)malloc(sizeof(Graph)); 
  g->adjacencyList = (Vertex**)malloc(v * sizeof(Vertex*));
  g->num_vertices = v;
  for (int i = 0; i < v; i++) {
    *(g->adjacencyList) = NULL;
  }

  return g;
}


void
destruct_graph(Graph *g) {
  Vertex *v;
  Vertex *next;
  for (int i = 0; i < g->num_vertices; i++) {
    if (!g->adjacencyList[i])
      continue;
    v = g->adjacencyList[i];
    next = v->n;
    while (v) {
      free(v);
      v = next;
      if (next)
        next = next->n;
    }
  }
  free(g->adjacencyList);
  free(g);
}

void
add_vertices(Graph *g, int v) {
}

void
add_edge(Graph *g, Edge e) {
  Vertex *v = (Vertex*)malloc(sizeof(Vertex));
  v->n = g->adjacencyList[e.u];
  v->v = e.v;
  g->adjacencyList[e.u] = v;
}

void add_edges(Graph *g, Edge **e, size_t size) {
  for (size_t i = 0; i < size; i++)
    add_edge(g, *e[i]);
}

void erase_edge(Graph *g, Edge e);
void erase_vertex(Graph *g, int v);
void has_edge(Graph *g, Edge *e);
Neighborhood
neighbors(Graph *g, int v) {
  Neighborhood n = {(const int*)&g->adjacencyList[v]->v, 1};
  return n;
}

Neighborhood next_neighbors(Graph *g, Neighborhood n) {
  Neighborhood n2 = {(const int*)&((Vertex*)(n.neighbors+1))->v, 1};
  return n2;
}

void
print_graph(Graph *g) {
  Vertex *v;
  for (int i = 0; i < g->num_vertices; i++) {
    printf("\n%d ", i);
    v = g->adjacencyList[i];
    while (v) {
      printf("%d ", v->v);
      v = v->n;
    }
  }
}
