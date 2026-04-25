#include "../src/graph.h"
#include <stdio.h>

int main() {
  Graph *g = create_graph(1);

  printf("ok0");
  add_vertex(g, 1);
  assert_vertex(has_vertex, g, 1);
  printf("ok1");

  add_vertex(g, 2);
  assert_vertex(has_vertex, g, 2);
  printf("ok2");

  add_vertex(g, 5);
  assert_vertex(has_vertex, g, 5);
  printf("ok3");

  add_vertex(g, 4);
  assert_vertex(has_vertex, g, 4);

  printf("ok4");
  destruct_graph(g);

  return 0;
}
