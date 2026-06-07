#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "graph.h"

struct Graph {
  int num_vertices;
  int *adjacencyMatrix;
  int *vertices;
};

Graph
*create_graph(int v) {
  Graph *g = (Graph*)malloc(sizeof(Graph));
  g->num_vertices = v;
  g->adjacencyMatrix = (int*)calloc(v * v, sizeof(int));
  if (!g->adjacencyMatrix) {
    printf("error on allocation\n");
    printf("%s\n", strerror(errno));
    exit(0);
  }

  for (int i = 0; i < v * v; i++)
    g->adjacencyMatrix[i] = 0;

  g->vertices = (int*)calloc(v, sizeof(int));

  return g;
}

void destruct_graph(Graph *g) {
  free(g->adjacencyMatrix);
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

  if (g->num_vertices > v) {
    g->vertices[v] = 0;
    return;
  }

  int *auxiliarMatrix = (int*)calloc((v+1) * (v+1), sizeof(int));
  for (int i = 0; i < g->num_vertices; i++)
    memcpy(&auxiliarMatrix[i*(v+1)], &g->adjacencyMatrix[i*(g->num_vertices)], g->num_vertices);

  free(g->adjacencyMatrix);
  g->adjacencyMatrix = auxiliarMatrix;

  g->vertices = realloc(g->vertices, sizeof(int) * (v+1));
  for (int i = g->num_vertices; i < v; i++)
    g->vertices[i] = 1;
  g->vertices[v] = 0;

  g->num_vertices = v+1;
}

int
has_vertex(Graph *g, int v) {
  if (v >= g->num_vertices || v < 0)
    return 0;

  return !g->vertices[v];
}

void
add_edge(Graph *g, Edge e) {
  if (e.u >= g->num_vertices || e.v >= g->num_vertices || g->vertices[e.u] || g->vertices[e.v])
    return;
  g->adjacencyMatrix[e.u*g->num_vertices + e.v] = 1;
}

void
add_edges(Graph *g, Edge **e, size_t size) {
  for (size_t i = 0; i < size; i++)
    add_edge(g, *(e[i]));
}

void
erase_edge(Graph *g, Edge e) {
  if (e.u >= g->num_vertices || e.v >= g->num_vertices || g->vertices[e.u] || g->vertices[e.v])
    return;
  g->adjacencyMatrix[e.u*g->num_vertices + e.v] = 0;
}

void erase_vertex(Graph *g, int v) {
  if (!has_vertex(g, v))
    return;
  g->vertices[v] = 1;
    
  for (int i = 0; i < g->num_vertices; i++) {
    g->adjacencyMatrix[v*g->num_vertices + i] = 0;
    g->adjacencyMatrix[i*g->num_vertices + v] = 0;
  }
}

int
has_edge(Graph *g, Edge e) {
  if (e.u >= g->num_vertices || e.v >= g->num_vertices || g->vertices[e.u] || g->vertices[e.v])
    return 0;

  return g->adjacencyMatrix[e.u*g->num_vertices + e.v];
}

Neighborhood
neighbors(Graph *g, int v) {
  int num_neighbors = 0;
  for (int i = 0; i < g->num_vertices; i++)
    if (g->adjacencyMatrix[v*g->num_vertices + i])
      num_neighbors++;

  int *vertex_neighbors = (int*)malloc(sizeof(int) * num_neighbors);
  for (int i = 0, j = 0; i < g->num_vertices; i++)
    if (g->adjacencyMatrix[v*g->num_vertices + i]) {
      vertex_neighbors[j] = i;
      j++;
    }
     
  Neighborhood n = {(const int *)vertex_neighbors, num_neighbors};
  return n;
}

Neighborhood
next_neighbors(Graph *g, Neighborhood n) {
  destruct_neighborhood(n);
  return (Neighborhood){NULL, 0};
}

void
destruct_neighborhood(Neighborhood n) {
  free((int*)n.neighbors);
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
