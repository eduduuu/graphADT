#include "../src/graph.h"
#include <stdio.h>

int main() {
  Graph *g = create_graph(1);
  printf("ok1");
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 0);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);

  printf("ok2");
  add_vertex(g, 1);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);

  printf("ok3");
  add_vertex(g, 3);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 1);

  printf("ok4");
  add_vertex(g, 2);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 1);
  assert_vertex(has_vertex, g, 3, 1);

  printf("ok5");
  destruct_graph(g);

  return 0;
}
