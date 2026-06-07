#include <stdio.h>
#include "../src/graph.h"

int main() {
  Graph *p_g = create_graph(3);

  printf("OK 1\n");
  add_edge(p_g, (Edge){0, 1});
  printf("OK a\n");
  fflush(stdout);
  assert_edge(has_edge, p_g, (Edge){0, 0}, 0);
  printf("OK a\n");
  fflush(stdout);
  assert_edge(has_edge, p_g, (Edge){0, 1}, 1);
  printf("OK a\n");
  fflush(stdout);
  assert_edge(has_edge, p_g, (Edge){0, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 2}, 0);

  printf("OK 2\n");
  add_edge(p_g, (Edge){0, 2});
  assert_edge(has_edge, p_g, (Edge){0, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){0, 1}, 1);
  assert_edge(has_edge, p_g, (Edge){0, 2}, 1);
  assert_edge(has_edge, p_g, (Edge){1, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 2}, 0);

  printf("OK 3\n");
  add_edge(p_g, (Edge){1, 0});
  assert_edge(has_edge, p_g, (Edge){0, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){0, 1}, 1);
  assert_edge(has_edge, p_g, (Edge){0, 2}, 1);
  assert_edge(has_edge, p_g, (Edge){1, 0}, 1);
  assert_edge(has_edge, p_g, (Edge){2, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 2}, 0);

  printf("OK 4\n");
  add_edge(p_g, (Edge){2, 2});
  assert_edge(has_edge, p_g, (Edge){0, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){0, 1}, 1);
  assert_edge(has_edge, p_g, (Edge){0, 2}, 1);
  assert_edge(has_edge, p_g, (Edge){1, 0}, 1);
  assert_edge(has_edge, p_g, (Edge){2, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 2}, 1);

  destruct_graph(p_g);
  printf("OK\n");
  return 0;
}
