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
  if (has_vertex(g, v))
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
  if (v >= g->num_vertices)
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

void
add_edges(Graph *g, Edge *e, size_t size) {
  g->edges_array = realloc(g->edges_array, g->num_edges + size);

  int indexOldEdges = g->num_edges - 1;
  int indexNewEdges = size -1;
  int indexVertex = g->num_vertices - 1;
  for (int i = g->num_edges + size - 1; i >= 0; i--) {
    if (e[indexNewEdges].u == indexVertex) {
      g->edges_array[i] = e[indexNewEdges].v;
      indexNewEdges--;
    } else {
      if (indexNewEdges >= g->vertex_array[indexVertex]) {
        g->edges_array[i] = g->edges_array[indexNewEdges];
        indexNewEdges--;
      }
      if (indexNewEdges < g->vertex_array[indexVertex]) {
        g->vertex_array[indexVertex] = i;
        indexVertex--;
      }
    }
  }
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
    if (g->num_edges == 1) {
      free(g->edges_array);
    } else if (edge_index != g->num_edges - 1) {
      int *new_edges_array = (int*)malloc(sizeof(int) * (g->num_edges - 1));
      memcpy(new_edges_array, g->edges_array, sizeof(int) * edge_index);
      memcpy(&new_edges_array[edge_index], &g->edges_array[edge_index+1], sizeof(int) * (g->num_edges - edge_index-1));
      free(g->edges_array);
      g->edges_array = new_edges_array;
    } else {
      g->edges_array = (int*)realloc(g->edges_array, sizeof(int) * (g->num_edges-1));
    }
    
    for (int i = e.u+1; i < g->num_vertices; i++)
      g->vertex_array[i]--;
    
    g->num_edges--;
  }
}

void
erase_vertex(Graph *g, int v) {
  if (!has_vertex(g, v))
    return;

  g->vertices[v] = 1;
  if (g->num_edges == 0)
    return;

  int edge_index = 0, begin_index = 0, end_index;
  for (int i = 0; i < g->num_vertices; i++) {
    if (i != v) {
      if (i == g->num_vertices-1)
        end_index = g->num_edges;
      else
        end_index = g->vertex_array[i+1];
      for (int j = begin_index; j < end_index; j++) {
        if (g->edges_array[j] != v) {
          g->edges_array[edge_index] = g->edges_array[j];
          edge_index++;
        }
      }
    }
    
    if (i != g->num_vertices-1) {
      begin_index = g->vertex_array[i+1];
      g->vertex_array[i+1] = edge_index;
    }
  }

  if (edge_index != 0 && end_index != g->num_edges) {
    g->edges_array = (int*)realloc(g->edges_array, sizeof(int) * edge_index);
  } else if (edge_index == 0) {
    free(g->edges_array);
    g->edges_array = NULL;
  }
  g->num_edges = edge_index;
}

Neighborhood neighbors(Graph *g, int v) {
  Neighborhood n;
  if (!has_vertex(g, v)) {
    n.neighbors = NULL;
    n.blockSize = 0;
  } else {
    n.neighbors = &g->edges_array[g->vertex_array[v]];
    if (v == g->num_vertices - 1)
      n.blockSize = g->num_edges - g->vertex_array[v];
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