#include <stdio.h>
#include "../src/graph.h"

int main() {
  Graph *p_g = create_graph(3);
  
  printf("ok 1\n");
  add_edge(p_g, (Edge){0, 1});
  assert_edge(has_edge, p_g, (Edge){0, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){0, 1}, 1);
  assert_edge(has_edge, p_g, (Edge){0, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 2}, 0);

  printf("ok 2\n");
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

  printf("ok 3\n");
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
  
  printf("ok 4\n");
  erase_edge(p_g, (Edge){0, 1});
  assert_edge(has_edge, p_g, (Edge){0, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){0, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){0, 2}, 1);
  assert_edge(has_edge, p_g, (Edge){1, 0}, 1);
  assert_edge(has_edge, p_g, (Edge){2, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 2}, 0);

  printf("ok 5\n");
  erase_edge(p_g, (Edge){0, 2});
  assert_edge(has_edge, p_g, (Edge){0, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){0, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){0, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 0}, 1);
  assert_edge(has_edge, p_g, (Edge){2, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 2}, 0);

  printf("ok 6\n");
  erase_edge(p_g, (Edge){0, 2});
  assert_edge(has_edge, p_g, (Edge){0, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){0, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){0, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 0}, 1);
  assert_edge(has_edge, p_g, (Edge){2, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 2}, 0);

  printf("ok 7\n");
  add_edge(p_g, (Edge){0, 1});
  assert_edge(has_edge, p_g, (Edge){0, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){0, 1}, 1);
  assert_edge(has_edge, p_g, (Edge){0, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 0}, 1);
  assert_edge(has_edge, p_g, (Edge){2, 0}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 2}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){1, 1}, 0);
  assert_edge(has_edge, p_g, (Edge){2, 2}, 0);

  destruct_graph(p_g);
  printf("ok\n");

  return 0;
}
