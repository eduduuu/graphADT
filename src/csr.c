#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "graph.h"

struct Graph {
    int num_vertices;
    int num_edges;
    int *vertex_array;
    int *edges_array;
    int *vertices;
};

Graph*
create_graph(int v) {
  Graph *p_g = (Graph*)malloc(sizeof(Graph));
  if (v > 0) {
    p_g->vertex_array = malloc(sizeof(int) * (v+1));
    p_g->vertices = calloc((v+1), sizeof(int));
  } else {
    p_g->vertex_array = NULL;
    p_g->edges_array = NULL;
  }
  p_g->num_vertices = v;
  p_g->num_edges = 0;
  p_g->edges_array = NULL;

  return p_g;
}

void
destruct_graph(Graph *g) {
  if (g->vertex_array) {
    free(g->vertex_array);
    free(g->vertices);
  }
  if (g->edges_array)
    free(g->edges_array);
  free(g);
}

void
add_vertex(Graph *g, int v) {
  if (g->num_vertices > v) {
    g->vertices[v] = 0;
    return;
  }

  g->vertex_array = (int*)realloc(g->vertex_array, sizeof(int) * (v+1));
  g->vertices = realloc(g->vertices, sizeof(int) * (v+1));
  for (int i = g->num_vertices; i < v; i++) {
    if (g->num_vertices)
      g->vertex_array[i] = g->vertex_array[i-1];
    else
      g->vertex_array[i] = 0;

    g->vertices[i] = 1;
  }
  g->vertices[v] = 0;
  if (g->num_vertices)
    g->vertex_array[v] = g->vertex_array[v-1];
  else
    g->vertex_array[v] = 0;

  g->num_vertices = v+1;
}

int
has_vertex(Graph *g, int v) {
  if (v >= g->num_vertices)
    return 0;
  return !g->vertices[v];
}

void
add_edge(Graph *g, Edge e) {
 if (!has_vertex(g, e.u) || !has_vertex(g, e.v) || has_edge(g, e))
    return;

  g->edges_array = (int*)realloc(g->edges_array, sizeof(int) * (g->num_edges+1));
  g->num_edges++;
  if (e.u == g->num_vertices-1) {
    g->edges_array[g->num_edges-1] = e.v;
    return;
  }

  int startNextVertex = g->vertex_array[e.u+1];
  int size;
  if (e.u+1 == g->num_vertices-1)
    size = g->num_edges - startNextVertex;
  else
    size = g->vertex_array[e.u+2] - startNextVertex;
  memcpy(&g->edges_array[startNextVertex+1], &g->edges_array[startNextVertex], size);

  g->edges_array[startNextVertex] = e.v;

  for (int i = e.u+1; i < g->num_vertices; i++)
    g->vertex_array[i]++;
}

void
add_edges(Graph *g, Edge **e, size_t size) {
  for (size_t i = 0; i < size; i++)
    add_edge(g, *(e[i]));
}

int has_edge(Graph *g, Edge e) {
  if (!has_vertex(g, e.u) || !has_vertex(g, e.v))
    return 0;

  int index = g->vertex_array[e.u];
  int end;
  if (e.u == g->num_vertices - 1)
    end = g->num_edges;
  else
    end = g->vertex_array[e.u+1];
  for (; index < end; index++)
    if (g->edges_array[index] == e.v)
      return 1;

  return 0;
}


void
erase_edge(Graph *g, Edge e) {
  if (!has_edge(g, e))
    return;
  
  int edge_index = g->vertex_array[e.u];
  int next_vertex_edges;
  if (e.v == g->num_vertices-1)
    next_vertex_edges = g->num_edges;
  else
    next_vertex_edges = g->vertex_array[e.u+1];

  for (; edge_index < next_vertex_edges; edge_index++)
    if (g->edges_array[edge_index] == e.v)
      break;

  if (edge_index != g->num_edges-1) {
    memcpy(&g->edges_array[edge_index], &g->edges_array[edge_index+1], sizeof(int) * (g->num_edges - edge_index));
    g->edges_array = (int*)realloc(g->edges_array, --g->num_edges);
  }

  for (int i = e.u+1; i < g->num_vertices; i++)
    g->vertex_array[i]--;
}

void
erase_vertex(Graph *g, int v) {
  if (!has_vertex(g, v))
    return;

  g->vertices[v] = 1;
  int num_edges_remove;

  if (v != g->num_vertices - 1) {
    int *vertex_edges = &g->edges_array[g->vertex_array[v]];
    int *next_vertex_edges = &g->edges_array[g->vertex_array[v+1]];
    num_edges_remove = next_vertex_edges - vertex_edges;
    memcpy(vertex_edges, next_vertex_edges, sizeof(int) * num_edges_remove);
  } else {
    num_edges_remove = g->num_edges - g->vertex_array[v];
  }
  g->edges_array = (int*)realloc(g->edges_array, num_edges_remove);
}

Neighborhood neighbors(Graph *g, int v) {
  Neighborhood n;
  if (!has_vertex(g, v)) {
    n.neighbors = NULL;
    n.blockSize = 0;
  } else {
    n.neighbors = &g->edges_array[g->vertex_array[v]];
    if (v == g->num_vertices - 1)
      n.blockSize = g->num_edges = g->vertex_array[v];
    else
      n.blockSize = g->vertex_array[v+1] - g->vertex_array[v];
  }

  return n;
}

Neighborhood next_neighbors(Graph *g, Neighborhood n) {
  return (Neighborhood){ NULL, 0 };
}

void destruct_neighborhood(Neighborhood n) {
  return;
}

void print_graph(Graph *g) {

}