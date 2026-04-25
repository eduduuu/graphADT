#include "../src/graph.h"
#include <stdio.h>

int main() {
  Graph *g = create_graph(1);

  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 0);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);

  add_vertex(g, 1);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);

  add_vertex(g, 3);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 1);

  erase_vertex(g, 1);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 0);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 1);

  erase_vertex(g, 3);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 0);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);
  destruct_graph(g);

  return 0;
}
