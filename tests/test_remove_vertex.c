#include "../src/graph.h"
#include <stdio.h>

int main() {
  Graph *g = create_graph(1);

  printf("Ok 1\n");
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 0);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);

  printf("Ok 2\n");
  add_vertex(g, 1);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);

  printf("Ok 3\n");
  add_vertex(g, 3);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 1);

  add_edge(g, (Edge){0, 1});
  add_edge(g, (Edge){1, 0});
  assert_edge(has_edge, g, (Edge){0, 1}, 1);
  assert_edge(has_edge, g, (Edge){1, 0}, 1);

  printf("Ok 4\n");
  erase_vertex(g, 1);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 0);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 1);

  assert_edge(has_edge, g, (Edge){0, 1}, 0);
  assert_edge(has_edge, g, (Edge){1, 0}, 0);
  
  printf("Ok 5\n");
  erase_vertex(g, 3);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 0);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);
  
  printf("Ok 6\n");
  erase_vertex(g, 1);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 0);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);


  printf("Ok 7\n");
  erase_vertex(g, 1);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 0);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);
  destruct_graph(g);

  printf("Ok\n");
  return 0;
}
