#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "../src/graph.h"

static inline void rand_init(unsigned short *seed)
{
  seed[0] = (unsigned short)rand();
}
static inline int rand_range(int n, unsigned short *seed) {
  int v = (int)(erand48(seed) * n);
  assert (v >= 0 && v < n);
  return v;
}

typedef struct micro_bech {
  Graph *p_g;
  int vertex_insertions;
  int vertex_deletions;
  int edge_insertions;
  int edge_deletions;
  int neighbor_searches;
  int edge_searches;
  int operations[6];
  int op_count;
  unsigned short seed;
} micro_bench;

void
test_mb(micro_bench *p_mb) {
  int op, u, v;
  
  for (int i = p_mb->op_count; i > 0; i--) {
    op = rand_range(100, &p_mb->seed);
    if (op < p_mb->operations[0]) { // insert vertex
      u = rand_range(get_num_vertices(p_mb->p_g), &p_mb->seed);
      add_vertex(p_mb->p_g, u);
      p_mb->vertex_insertions+=1;
      //printf("add %d\n", u);
    } else if (op < p_mb->operations[1]) { // erase vertex
      u = rand_range(get_num_vertices(p_mb->p_g), &p_mb->seed);
      erase_vertex(p_mb->p_g, u);
      p_mb->vertex_deletions++;
     // printf("remove %d\n", u);
    } else if (op < p_mb->operations[2]) { // insert edge
      u = rand_range(get_num_vertices(p_mb->p_g), &p_mb->seed);
      v = rand_range(get_num_vertices(p_mb->p_g), &p_mb->seed);
      add_edge(p_mb->p_g, (Edge){u, v});
      p_mb->edge_insertions++;
      //printf("add %d %d\n", u, v);
    } else if (op < p_mb->operations[3]) { // erase edge
      u = rand_range(get_num_vertices(p_mb->p_g), &p_mb->seed);
      v = rand_range(get_num_vertices(p_mb->p_g), &p_mb->seed);
      erase_edge(p_mb->p_g, (Edge){u, v});
      p_mb->edge_deletions++;
    } else if (op < p_mb->operations[4]) {
      u = rand_range(get_num_vertices(p_mb->p_g), &p_mb->seed);
      v = rand_range(get_num_vertices(p_mb->p_g), &p_mb->seed);
      has_edge(p_mb->p_g, (Edge){u, v});
      p_mb->edge_searches++;
    } else {

    }

    p_mb->op_count--;
  }
}

void main() {
  micro_bench *p_mb = (micro_bench*)malloc(sizeof(micro_bench));
  p_mb->p_g = create_graph(10000);
  srand((int)time(NULL));
  rand_init(&p_mb->seed);
  p_mb->vertex_insertions = 0;
  p_mb->vertex_deletions = 0;
  p_mb->edge_insertions = 0;
  p_mb->edge_deletions = 0;
  p_mb->neighbor_searches = 0;
  p_mb->edge_searches = 0;
  int operations[] = {10, 10, 60, 80, 100, 0};
  memcpy(p_mb->operations, operations, sizeof(operations));
 
  p_mb->op_count = 100000;
  
  struct timeval start, end;
  gettimeofday(&start, NULL); 
  test_mb(p_mb);
  gettimeofday(&end, NULL);
  destruct_graph(p_mb->p_g);
  
  int duration = (end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
  
  printf("Duration           : %d (ms)\n", duration);
  printf("# v insert txs     : %d (%f / s)\n", p_mb->vertex_insertions, p_mb->vertex_insertions * 1000.0 / duration);
  printf("# v del txs        : %lu (%f / s)\n", p_mb->vertex_deletions, p_mb->vertex_deletions * 1000.0 / duration);
  printf("# e inser txs      : %lu (%f / s)\n", p_mb->edge_insertions, p_mb->edge_insertions * 1000.0 / duration);
  printf("# e deltxs         : %lu (%f / s)\n", p_mb->edge_deletions, p_mb->edge_deletions * 1000.0 / duration);
  printf("# e searchtxs      : %lu (%f / s)\n", p_mb->edge_searches, p_mb->edge_searches * 1000.0 / duration);
  
}