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
add_vertex(Graph *g, int v) {
  if (g->num_vertices > v) {
    if (g->adjacencyList[v] && g->adjacencyList[v]->v == -1) {
      free(g->adjacencyList[v]);
      g->adjacencyList[v] = NULL;
    }

    return;
  }

  g->adjacencyList = (Vertex**)realloc(g->adjacencyList, v * sizeof(Vertex));
  int num_vertices = g->num_vertices;
  g->num_vertices = v+1;
  for (int i = num_vertices; i < v-1; i++)
    add_edge(g, (Edge){i, -1});
  g->adjacencyList[v] = NULL;
}

int
has_vertex(Graph *g, int v) {
  // If the number of vertices is greater or equal the vertex and it doesnt has an edge or the edge is not to -1
  //printf("%ld", g->adjacencyList[v]->v != -1);
  if (g->num_vertices > v && (!g->adjacencyList[v] || g->adjacencyList[v]->v != -1))
    return 1;
  return 0;
}

void
add_edge(Graph *g, Edge e) {
  if (g->num_vertices <= e.u || (g->adjacencyList[e.u] && g->adjacencyList[e.u]->v == -1))
    return;
  Vertex *v = (Vertex*)malloc(sizeof(Vertex));
  v->n = g->adjacencyList[e.u];
  v->v = e.v;
  g->adjacencyList[e.u] = v;
}

void add_edges(Graph *g, Edge **e, size_t size) {
  for (size_t i = 0; i < size; i++)
    add_edge(g, *e[i]);
}

void
erase_edge(Graph *g, Edge e) {
  if (g->num_vertices <= e.u || !has_vertex(g, e.u))
    return;

  Vertex *current = g->adjacencyList[e.u];
  Vertex *previous = NULL;
  while (current) {
    if (current->v == e.v) {
      if (previous)
        previous->n = current->n;
      else
        g->adjacencyList[e.u] = current->n; 
      
      free(current);
      return;
    }

    previous = current;
    current = current->n;
  }
}

void
erase_vertex(Graph *g, int v) {
  if (g->num_vertices <= v)
      return;

  for (int i = 0; i < g->num_vertices; i++)
    if (i != v)
      erase_edge(g, (Edge){i, v});

  if (!g->adjacencyList[v])
    add_edge(g, (Edge){v, -1});
  if (g->adjacencyList[v]->v == -1)
    return;
  Vertex *u;
  Vertex *next;
  u = g->adjacencyList[v];
  u->v = -1;
  u = u->n;
  if (!u)
    return;
  next = u->n;
  while (u) {
    free(u);
    u = next;
    if (next)
      next = next->n;
  }
}

int
has_edge(Graph *g, Edge e) {
  if (g->num_vertices < e.u)
    return 0;

  Vertex *v = g->adjacencyList[e.u];
  while (v) {
    if (v->v == e.v)
      return 1;
    v = v->n;
  }

  return 0;
}

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
