#include <stdio.h>
#include "graph.h"

Graph*
read_graph(char *fileName) {
  FILE *p_file = fopen(fileName, "r");
  int n;
  fscanf(p_file, "%d", &n);
  Graph *p_graph = create_graph(n);
  int u, v;
  while (fscanf(p_file, "%d%d", &u, &v) != EOF)
    add_edge(p_graph, (Edge){u, v});

  fclose(p_file);

  return p_graph;
}
