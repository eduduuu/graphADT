#include <stdio.h>
#include "../src/graph.h"

void assert_neighbors(Graph *p_g, int v, int *neighborsArray, int num_vertices) {
  Neighborhood n = neighbors(p_g, 0);
  while (n.neighbors) {
      printf("a");
    for (int i = 0; i < n.blockSize; i++) {
      if (neighborsArray[n.neighbors[i]]) {
        printf("nok 1 index: %d %d", i, n.neighbors[i]);
        destruct_graph(p_g);
        exit(1);
      }

      neighborsArray[n.neighbors[i]] = 1; 
    }
    n = next_neighbors(p_g, n);
  }

  for (int i = 0; i < num_vertices; i++) {
    if (neighborsArray[i] == 0) {
        printf("nok index: %d", i);
        destruct_graph(p_g);
        exit(1);
      }
  }
}

int main() {
  Graph *p_g = create_graph(5);

  add_edge(p_g, (Edge){0, 1});
  add_edge(p_g, (Edge){0, 4});
  add_edge(p_g, (Edge){0, 2});
  print_graph(p_g);

  int neighborsArray[] = {1, 0, 0, 1, 0};
  assert_neighbors(p_g, 0, neighborsArray, 5);

  destruct_graph(p_g);
  return 0;
}
