#include "../src/graph.h"
#include <stdio.h>

int main() {
  Graph *g = create_graph(1);
  printf("ok 1\n");
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 0);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);

  printf("ok 2\n");
  add_vertex(g, 1);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);

  printf("ok 3\n");
  add_vertex(g, 3);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 1);

  printf("ok 4\n");
  add_vertex(g, 2);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 1);
  assert_vertex(has_vertex, g, 3, 1);

  printf("ok 5\n");
  destruct_graph(g);

  g = create_graph(1);
  printf("ok 6\n");
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 0);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);

  printf("ok 7\n");
  add_vertex(g, 1);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 0);

  printf("ok 8\n");
  assert_edge(has_edge, g, (Edge){0, 0}, 0);
  assert_edge(has_edge, g, (Edge){0, 1}, 0);
  assert_edge(has_edge, g, (Edge){1, 0}, 0);
  assert_edge(has_edge, g, (Edge){1, 1}, 0);

  printf("ok 9\n");
  add_edge(g, (Edge){1, 0});
  assert_edge(has_edge, g, (Edge){0, 0}, 0);
  assert_edge(has_edge, g, (Edge){0, 1}, 0);
  assert_edge(has_edge, g, (Edge){1, 0}, 1);
  assert_edge(has_edge, g, (Edge){1, 1}, 0);

  printf("ok 10\n");
  add_vertex(g, 3);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 0);
  assert_vertex(has_vertex, g, 3, 1);

  printf("ok 11\n");
  assert_edge(has_edge, g, (Edge){0, 0}, 0);
  assert_edge(has_edge, g, (Edge){0, 1}, 0);
  assert_edge(has_edge, g, (Edge){0, 2}, 0);
  assert_edge(has_edge, g, (Edge){1, 0}, 1);
  assert_edge(has_edge, g, (Edge){2, 0}, 0);
  assert_edge(has_edge, g, (Edge){1, 2}, 0);
  assert_edge(has_edge, g, (Edge){2, 1}, 0);
  assert_edge(has_edge, g, (Edge){1, 1}, 0);
  assert_edge(has_edge, g, (Edge){2, 2}, 0);

  printf("ok 12\n");
  add_vertex(g, 2);
  assert_vertex(has_vertex, g, 0, 1);
  assert_vertex(has_vertex, g, 1, 1);
  assert_vertex(has_vertex, g, 2, 1);
  assert_vertex(has_vertex, g, 3, 1);

  printf("ok 13\n");
  assert_edge(has_edge, g, (Edge){0, 0}, 0);
  assert_edge(has_edge, g, (Edge){0, 1}, 0);
  assert_edge(has_edge, g, (Edge){0, 2}, 0);
  assert_edge(has_edge, g, (Edge){1, 0}, 1);
  assert_edge(has_edge, g, (Edge){2, 0}, 0);
  assert_edge(has_edge, g, (Edge){1, 2}, 0);
  assert_edge(has_edge, g, (Edge){2, 1}, 0);
  assert_edge(has_edge, g, (Edge){1, 1}, 0);
  assert_edge(has_edge, g, (Edge){2, 2}, 0);


  printf("ok\n");
  destruct_graph(g);

  return 0;
}
