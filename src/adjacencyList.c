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
  int *vertices;
};

Graph*
create_graph(int v) {
  Graph *g = (Graph*)malloc(sizeof(Graph)); 
  g->adjacencyList = (Vertex**)malloc(v  * sizeof(Vertex*));
  g->num_vertices = v;
  for (int i = 0; i < v; i++) {
    g->adjacencyList[i] = NULL;
  }

  g->vertices = (int*)calloc(v, sizeof(int));

  return g;
}

void
destruct_graph(Graph *g) {
  Vertex *v;
  Vertex *next;

  for (int i = 0; i < g->num_vertices; i++) {
    if (g->vertices[i] || !g->adjacencyList[i])
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
  free(g->vertices);
  free(g);
}

int
get_num_vertices(Graph *g) {
  return g->num_vertices;
}

void
add_vertex(Graph *g, int v) {
  if (has_vertex(g, v))
    return;

  if (g->num_vertices <= v) {
    g->adjacencyList = (Vertex**)realloc(g->adjacencyList, (v+1) * sizeof(Vertex));
    int num_vertices = g->num_vertices;
    for (int i = num_vertices; i <= v; i++) {
      g->vertices[i] = 1;
      g->adjacencyList[i] = NULL;
    }
    g->num_vertices = v+1;
  }

  g->vertices[v] = 0;
}

int
has_vertex(Graph *g, int v) {
  if (v >= g->num_vertices || v < 0)
    return 0;

  return !g->vertices[v];
}

void
add_edge(Graph *g, Edge e) {
  if (!has_vertex(g, e.u) || !has_vertex(g, e.v) || has_edge(g, e))
    return;
  Vertex *v = (Vertex*)malloc(sizeof(Vertex));
  v->n = g->adjacencyList[e.u];
  v->v = e.v;
  g->adjacencyList[e.u] = v;
}

void add_edges(Graph *g, Edge *e, size_t size) {
  for (size_t i = 0; i < size; i++) {
    Vertex *v = (Vertex*)malloc(sizeof(Vertex));
    v->n = g->adjacencyList[e[i].u];
    v->v = e[i].v;
    g->adjacencyList[e[i].u] = v;
  }
}

void
erase_edge(Graph *g, Edge e) {
  if (!has_vertex(g, e.u) || !has_edge(g, e))
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

  current = g->adjacencyList[e.v];
  previous = NULL;
  while (current) {
    if (current->v == e.u) {
      if (previous)
        previous->n = current->n;
      else
        g->adjacencyList[e.v] = current->n; 
      
      free(current);
      return;
    }

    previous = current;
    current = current->n;
  }
}

void
erase_vertex(Graph *g, int v) {
  if (!has_vertex(g, v))
      return;

  g->vertices[v] = 1;
  
  for (int i = 0; i < g->num_vertices; i++)
    if (i != v)
      erase_edge(g, (Edge){i, v});

  Vertex *u;
  Vertex *next;
  u = g->adjacencyList[v];
  if (!u)
    return;
  next = u->n;
  while (u) {
    free(u);
    u = next;
    if (next)
      next = next->n;
  }

  g->adjacencyList[v] = NULL;
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
  if (!has_vertex(g, v))
    return (Neighborhood){NULL, 0};
  Neighborhood n = {(const int*)&(g->adjacencyList[v]->v), 1};
  return n;
}

Neighborhood next_neighbors(Graph *g, Neighborhood n) {
  Neighborhood n2 = {(const int*)&(((Vertex*)n.neighbors)->n->v), 1};
  return n2;
}

void destruct_neighborhood(Neighborhood n) {
  return;
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