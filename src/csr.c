#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
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
    p_g->vertex_array = calloc(v, sizeof(int));
    p_g->vertices = calloc(v, sizeof(int));
  } else {
    p_g->vertex_array = NULL;
    p_g->vertices = NULL;
  }

  p_g->num_vertices = v;
  p_g->num_edges = 0;
  p_g->edges_array = NULL;

  return p_g;
}

void
destruct_graph(Graph *g) {
  free(g->vertex_array);
  free(g->vertices);
  free(g->edges_array);
  free(g);
}

int
get_num_vertices(Graph *g) {
  return g->num_vertices;
}

void
add_vertex(Graph *g, int v) {
  if (v < 0 || has_vertex(g, v))
    return;

  if (g->num_vertices > v) {
    g->vertices[v] = 0;
    return;
  }

  g->vertex_array = (int*)realloc(g->vertex_array, sizeof(int) * (v+1));
  g->vertices = realloc(g->vertices, sizeof(int) * (v+1));
  for (int i = g->num_vertices; i <= v; i++) {
    g->vertex_array[i] = g->num_edges;
    g->vertices[i] = 1;
  }
  g->vertices[v] = 0;
  g->num_vertices = v+1;
}

int
has_vertex(Graph *g, int v) {
  if (v < 0 || v >= g->num_vertices)
    return 0;
  return !g->vertices[v];
}

void
add_edge(Graph *g, Edge e) {
 if (!has_vertex(g, e.u) || !has_vertex(g, e.v) || has_edge(g, e))
    return;


  g->num_edges++;
  if (e.u == g->num_vertices-1) {
    if (g->num_edges > 1) {
      g->edges_array = (int*)realloc(g->edges_array, sizeof(int) * g->num_edges);
    } else {
      g->edges_array = (int*)malloc(sizeof(int) * g->num_edges);
    }
    g->edges_array[g->num_edges-1] = e.v;
    return;
  }

  int *new_edges_array = (int*)malloc(sizeof(int) * g->num_edges);
  
  if (g->num_edges > 1) {
    int startNextVertex = g->vertex_array[e.u+1];
    assert(startNextVertex < g->num_edges);
    int size = g->num_edges - startNextVertex - 1;
    assert(startNextVertex + size < g->num_edges);
    
    memcpy(new_edges_array, g->edges_array, sizeof(int) * startNextVertex);
    memcpy(&new_edges_array[startNextVertex+1], &g->edges_array[startNextVertex], sizeof(int) * size);
    
    new_edges_array[startNextVertex] = e.v;
    
    free(g->edges_array);
  } else {
    new_edges_array[0] = e.v;
  }
  g->edges_array = new_edges_array;

  for (int i = e.u+1; i < g->num_vertices; i++)
    g->vertex_array[i]++;
}

// Bulk insert. Edges may come in any order. Edges with a missing endpoint or that
// already exist in the graph are skipped; duplicates inside the batch are the caller's responsibility.
void
add_edges(Graph *g, Edge *e, size_t size) {
  int n = g->num_vertices;
  if (size == 0 || n == 0)
    return;

  size_t *added = (size_t*)calloc(n, sizeof(size_t));    // new edges per vertex
  size_t *write = (size_t*)malloc(n * sizeof(size_t));   // write cursor per vertex
  unsigned char *accepted = (unsigned char*)calloc(size, 1);
  int *new_array = NULL;
  size_t total_new = 0;

  if (!added || !write || !accepted)
    goto done;

  for (size_t i = 0; i < size; i++) {
    if (has_vertex(g, e[i].u) && has_vertex(g, e[i].v) && !has_edge(g, e[i])) {
      accepted[i] = 1;
      added[e[i].u]++;
      total_new++;
    }
  }

  if (total_new == 0)
    goto done;

  new_array = (int*)malloc(sizeof(int) * ((size_t)g->num_edges + total_new));
  if (!new_array)
    goto done;

  // Copy the old edges to their new positions and compute the new vertex starts
  size_t pos = 0;
  for (int u = 0; u < n; u++) {
    size_t old_begin = g->vertex_array[u];
    size_t old_end = (u == n-1) ? (size_t)g->num_edges : (size_t)g->vertex_array[u+1];
    size_t old_count = old_end - old_begin;

    if (old_count)
      memcpy(&new_array[pos], &g->edges_array[old_begin], old_count * sizeof(int));

    g->vertex_array[u] = (int)pos;
    write[u] = pos + old_count;
    pos += old_count + added[u];
  }

  // Place the new edges after the old ones of their source vertex
  for (size_t i = 0; i < size; i++)
    if (accepted[i])
      new_array[write[e[i].u]++] = e[i].v;

  free(g->edges_array);
  g->edges_array = new_array;
  g->num_edges += (int)total_new;

done:
  free(added);
  free(write);
  free(accepted);
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

static void
shrink_edges(Graph *g) {
  // g->num_edges is already the new size
  if (g->num_edges == 0) {
    free(g->edges_array);
    g->edges_array = NULL;
  } else {
    int *p = (int*)realloc(g->edges_array, sizeof(int) * g->num_edges);
    if (p)
      g->edges_array = p;
  }
}

void
erase_edge(Graph *g, Edge e) {
  if (!has_vertex(g, e.u) || !has_vertex(g, e.v))
    return;
  
  int edge_index = g->vertex_array[e.u];
  int next_vertex_edges;
  if (e.u == g->num_vertices-1)
    next_vertex_edges = g->num_edges;
  else
    next_vertex_edges = g->vertex_array[e.u+1];

  for (; edge_index < next_vertex_edges; edge_index++)
    if (g->edges_array[edge_index] == e.v)
      break;

  if (edge_index != next_vertex_edges) {
    memmove(&g->edges_array[edge_index], &g->edges_array[edge_index+1],
            sizeof(int) * (g->num_edges - edge_index - 1));

    for (int i = e.u+1; i < g->num_vertices; i++)
      g->vertex_array[i]--;

    g->num_edges--;
    shrink_edges(g);
  }
}

void
erase_vertex(Graph *g, int v) {
  if (!has_vertex(g, v))
    return;

  g->vertices[v] = 1;
  if (g->num_edges == 0)
    return;

  // Compact the edge array in place: drop v's out-edges and every edge pointing to v.
  int write = 0, old_begin = 0;
  for (int i = 0; i < g->num_vertices; i++) {
    int old_end = (i == g->num_vertices-1) ? g->num_edges : g->vertex_array[i+1];
    int new_begin = write;

    if (i != v) {
      for (int j = old_begin; j < old_end; j++) {
        if (g->edges_array[j] != v)
          g->edges_array[write++] = g->edges_array[j];
      }
    }

    g->vertex_array[i] = new_begin;
    old_begin = old_end;
  }

  g->num_edges = write;
  shrink_edges(g);
}

Neighborhood neighbors(Graph *g, int v) {
  Neighborhood n = {NULL, 0};
  if (!has_vertex(g, v))
    return n;

  size_t begin = g->vertex_array[v];
  size_t end = (v == g->num_vertices - 1) ? (size_t)g->num_edges : (size_t)g->vertex_array[v+1];
  if (end > begin) {
    n.neighbors = &g->edges_array[begin];
    n.blockSize = end - begin;
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
