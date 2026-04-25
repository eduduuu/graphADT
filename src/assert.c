#include <stdlib.h>
#include "graph.h"

int assert_vertex(int (*p_func)(Graph*, int), Graph *p_g, int v) {
  int returnValue = p_func(p_g, v);

  if (!returnValue) {
    destruct_graph(p_g);
    exit(1);
  }

  return 0;
}

int assert_edge(int (*p_func)(Graph*, Edge), Graph *p_g, Edge e) {
  int returnValue = p_func(p_g, e);

  if (!returnValue) {
    destruct_graph(p_g);
    exit(1);
  }

  return 0;
}
