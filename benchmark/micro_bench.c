#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <getopt.h>
#include <math.h>
#include "../src/graph.h"

static inline void rand_init(unsigned short *seed)
{
  seed[0] = (unsigned short)rand();
  seed[1] = (unsigned short)rand();
  seed[2] = (unsigned short)rand();
}

static inline int rand_range(int n, unsigned short *seed) {
  int v = (int)(erand48(seed) * n);
  //assert (v >= 0 && v < n);
  return v;
}

typedef struct micro_bech {
  Graph *p_g;
  int *op_array;
  int vertex_insertions;
  int vertex_deletions;
  int edge_insertions;
  int edge_deletions;
  int edge_searches;
  int op_count;
  int duration;
  int op_distribution[3];
  unsigned short seed[3];
} micro_bench;

static volatile int stop;
static void*
test_random(void *p_mb_) {
  micro_bench *p_mb = (micro_bench*)p_mb_;
  int op, u, v;

  while (stop == 0) {
    op = rand_range(100, p_mb->seed);
    // vertex update
    if (op < p_mb->op_distribution[0]) {
      u = rand_range(get_num_vertices(p_mb->p_g), p_mb->seed);
      if (has_vertex(p_mb->p_g, u)) {
        erase_vertex(p_mb->p_g, u);
        p_mb->vertex_deletions++;
      } else {
        add_vertex(p_mb->p_g, u);
        p_mb->vertex_insertions+=1;
      }
      
    // edge update
    } else if (op < p_mb->op_distribution[1]) {
      u = rand_range(get_num_vertices(p_mb->p_g), p_mb->seed);
      v = rand_range(get_num_vertices(p_mb->p_g), p_mb->seed);
      if (!has_edge(p_mb->p_g, (Edge){u, v})) {
        add_edge(p_mb->p_g, (Edge){u, v});
        p_mb->edge_insertions++;
      } else {
        erase_edge(p_mb->p_g, (Edge){u, v});
        p_mb->edge_deletions++;
      }
    // edge search
    } else if (op < p_mb->op_distribution[2]) {
      u = rand_range(get_num_vertices(p_mb->p_g), p_mb->seed);
      v = rand_range(get_num_vertices(p_mb->p_g), p_mb->seed);
      has_edge(p_mb->p_g, (Edge){u, v});
      p_mb->edge_searches++;
    }
  }
}

void run_test(micro_bench *p_mb, void*(test_func)(void*)) {
  stop = 0;

  struct timeval start, end;
  struct timespec timeout;

  timeout.tv_sec = p_mb->duration / 1000;
  timeout.tv_nsec = (p_mb->duration % 1000) * 1000000;

  pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t));
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_create(thread, &attr, test_func, (void*)p_mb);
  pthread_setname_np(*thread, "bech_thread");

  gettimeofday(&start, NULL);
  nanosleep(&timeout, NULL);

  stop = 1;

  gettimeofday(&end, NULL);
  pthread_join(*thread, NULL);
  free(thread);

  p_mb->duration = (end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
}

int choose_vertex(Graph *g, unsigned short seed[3]) {
  int u = rand_range(get_num_vertices(g), seed);
  int v;
  for (v = u; v < get_num_vertices(g); v++)
    if (has_vertex(g, v))
      break;
  if (v == get_num_vertices(g))
    for (v = u; v >= 0; v--)
      if (has_vertex(g, v))
        break;

  return v;
}

void choose_edge(Graph *g, unsigned short seed[3], int has, Edge* edge) {
  int u = rand_range(get_num_vertices(g), seed);
  int v = rand_range(get_num_vertices(g), seed);

  for (int i = u; i < get_num_vertices(g); i++) {
    for (int j = v; j < get_num_vertices(g); j++) {
      if (has_vertex(g, i) && has_vertex(g, j) && has_edge(g, (Edge){i, j}) == has) {
        *edge = (Edge){i, j};
        return;
      } 
    }
    for (int j = v; j >= 0; j--) {
      if (has_vertex(g, i) && has_vertex(g, j) && has_edge(g, (Edge){i, j}) == has) {
        *edge = (Edge){i, j};
        return;
      } 
    }
  }

  for (int i = u; i >= 0; i--) {
    for (int j = v; j < get_num_vertices(g); j++) {
      if (has_vertex(g, i) && has_vertex(g, j) && has_edge(g, (Edge){i, j}) == has) {
        *edge = (Edge){i, j};
        return;
      } 
    }
    for (int j = v; j >= 0; j--) {
      if (has_vertex(g, i) && has_vertex(g, j) && has_edge(g, (Edge){i, j}) == has) {
        *edge = (Edge){i, j};
        return;
      } 
    }
  }
}

static void*
test(void *p_mb_) {
  micro_bench *p_mb = (micro_bench*)p_mb_;

  int index_op = 0;
  for (int i = 0; i < p_mb->op_count; i++) {
    if (stop)
      break;
    if (p_mb->op_array[index_op] == 0) {
      add_vertex(p_mb->p_g, p_mb->op_array[index_op+1]);
      p_mb->vertex_insertions++;
      index_op += 2;
    } else if (p_mb->op_array[index_op] == 1) {
      erase_vertex(p_mb->p_g, p_mb->op_array[index_op+1]);
      p_mb->vertex_deletions++;
      index_op += 2;
    } else if (p_mb->op_array[index_op] == 2) {
      add_edge(p_mb->p_g, (Edge){p_mb->op_array[index_op+1], p_mb->op_array[index_op+2]});
      p_mb->edge_insertions++;
      index_op += 3;
    } else if (p_mb->op_array[index_op] == 3) {
      erase_edge(p_mb->p_g, (Edge){p_mb->op_array[index_op+1], p_mb->op_array[index_op+2]});
      p_mb->edge_deletions++;
      index_op += 3;
    } else if (p_mb->op_array[index_op] == 4) {
      has_edge(p_mb->p_g, (Edge){p_mb->op_array[index_op+1], p_mb->op_array[index_op+2]});
      p_mb->edge_searches++;
      index_op += 3;
    }
  }

  if (!stop) {
    printf("Operations ended before the timer.");
    fflush(stdout);
  }
}

int* generate_operations(micro_bench *p_mb) {
  Graph *g = copy_graph(p_mb->p_g);
  int sample_duration = 1000, duration = p_mb->duration;
  p_mb->duration = sample_duration;

  
  run_test(p_mb, test_random);
  printf("Ran Sample\n");
  fflush(stdout);
  
  p_mb->duration = duration;
  double factor = (p_mb->duration / sample_duration) * 10;
  printf("factor: %f\n", factor);
  
  int vertex_update = ceil((p_mb->vertex_insertions + p_mb->vertex_deletions)* factor);
  int edge_update = ceil((p_mb->edge_insertions + p_mb->edge_deletions)* factor);
  int edge_searches = ceil(p_mb->edge_searches * factor);

  
  int size_op = vertex_update*2 + edge_update*3 + edge_searches*3;
  p_mb->op_array = (int*)malloc(sizeof(int) * size_op);
  p_mb->op_count = vertex_update + edge_update + edge_searches;
  
  destruct_graph(p_mb->p_g);

  p_mb->p_g = copy_graph(g);

  int op, u, index_op = 0, insert_edge = 1;
  Edge edge;

  while (1) {
    if (vertex_update + edge_update + edge_searches == 0)
      break;
    //printf("%d %d %d\n", vertex_update, edge_update, edge_searches);
    //fflush(stdout);
    op = rand_range(100, p_mb->seed);
    // vertex update
    if (op < p_mb->op_distribution[0]) {
      if (!vertex_update)
        continue;
      
      u = rand_range(get_num_vertices(g), p_mb->seed);
      if (!has_vertex(g, u)) {
        add_vertex(g, u);
        p_mb->op_array[index_op] = 0;
        p_mb->op_array[index_op+1] = u;
      } else {
        erase_vertex(g, u);
        p_mb->op_array[index_op] = 1;
        p_mb->op_array[index_op+1] = u;
      }
      
      vertex_update--;
      index_op += 2;

    // edge update
    } else if (op < p_mb->op_distribution[1]) {
      if (!edge_update)
        continue;

      choose_edge(g, p_mb->seed, insert_edge, &edge);
      if (insert_edge) {
        add_edge(g, edge);
        insert_edge = 0;
        p_mb->op_array[index_op] = 2;
        p_mb->op_array[index_op+1] = edge.u;
        p_mb->op_array[index_op+2] = edge.v;
      } else {
        insert_edge = 1;
        erase_edge(g, edge);
        p_mb->op_array[index_op] = 3;
        p_mb->op_array[index_op+1] = edge.u;
        p_mb->op_array[index_op+2] = edge.v;
      }
      
      edge_update--;
      index_op += 3;

    // search edge
    } else if (op < p_mb->op_distribution[2]) {
      if (!edge_searches)
        continue;

      choose_edge(g, p_mb->seed, 1, &edge);

      p_mb->op_array[index_op] = 4;
      p_mb->op_array[index_op+1] = edge.u;
      p_mb->op_array[index_op+2] = edge.v;
      
      edge_searches--;
      index_op += 3;
    }
  }

  p_mb->vertex_insertions = 0;
  p_mb->vertex_deletions = 0;
  p_mb->edge_deletions = 0;
  p_mb->edge_insertions = 0;
  p_mb->edge_searches = 0;

  destruct_graph(g);

  printf("Generated Operations\n");
  fflush(stdout);
}

int main(int argc, char **argv) {
  int duration = 1000;
  int size = 100;
  double density = 0.75;
  int operations[] = {25, 25, 50};
  int mode = 0;
  int factor = 1;
  double shuffle_factor = 0;
  char file_name[50];
  file_name[0] = '\0';

  /*
    t = test duration
    s = graph size
    d = graph density
    a = vertex update
    b = edge update
    c = edge search
    m = type of test
    f = factor
    w = file to write
    g = factor to shuffle edges
  */
  char opt;
  while ((opt = getopt(argc, argv, "t:s:d:a:b:c:m:f:w:g:")) != -1) {
    switch (opt) {
      case 't':
        printf("time: %s\n", optarg);
        duration = atoi(optarg);
        break;
      case 's':
        printf("size: %s\n", optarg);
        size = atoi(optarg);
        break;
      case 'd':
        density = atof(optarg);
        printf("density: %s\n", optarg);
        break;
      case 'a':
        operations[0] = atoi(optarg);
        break;
      case 'b':
        operations[1] = atoi(optarg);
        break;
      case 'c':
        operations[2] = atoi(optarg);
        break;
      case 'm':
        mode = atoi(optarg);
        break;
      case 'f':
        factor = atoi(optarg);
        break;
      case 'w':
        strcpy(file_name, optarg);
        break;
      case 'g':
        shuffle_factor = atoi(optarg);
        break;
    }
  }
  operations[1] = operations[0] + operations[1];
  operations[2] = operations[1] + operations[2];

  assert(operations[2] == 100);
  assert(size >= 1);
  assert(density > 0 && density <= 1);
  assert(mode == 0 || mode == 1);
  
  micro_bench *p_mb = (micro_bench*)malloc(sizeof(micro_bench));
  p_mb->p_g = generate_graph(size, density, shuffle_factor);
  printf("Generated Graph\n");
  fflush(stdout);
  
  //p_mb->p_g = create_graph(size);
  //srand((int)time(NULL));
  //rand_init(p_mb->seed);

  p_mb->vertex_insertions = 0;
  p_mb->vertex_deletions = 0;
  p_mb->edge_insertions = 0;
  p_mb->edge_deletions = 0;
  p_mb->edge_searches = 0;
  p_mb->duration = duration;

  
  p_mb->op_distribution[0] = operations[0];
  p_mb->op_distribution[1] = operations[1];
  p_mb->op_distribution[2] = operations[2];

  if (mode) {
    generate_operations(p_mb);
  
    run_test(p_mb, test);
  } else {
    run_test(p_mb, test_random);
  }

 
  /*
  stop = 0;
  
  struct timeval start, end;
  struct timespec timeout;

  timeout.tv_sec = duration / 1000;
  timeout.tv_nsec = (duration % 1000) * 1000000;

  pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t));
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_create(thread, &attr, test_random, (void*)p_mb);

  gettimeofday(&start, NULL);
  nanosleep(&timeout, NULL);
  stop = 1;

  gettimeofday(&end, NULL);
  pthread_join(*thread, NULL);
  
  destruct_graph(p_mb->p_g);
  pthread_attr_destroy(&attr);
  
  duration = (end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
  */
  if (file_name[0] != '\0') {
    int insert_header = access(file_name, F_OK);
    FILE *fptr = fopen(file_name, "a");

    if (insert_header) {
      fprintf(fptr, "size,duration,density,v_updates,e_updates,e_searchs,mode,factor,v_insert,v_del,e_insert,e_del,e_searches\n");
    }

    fprintf(fptr, "%d,%d,%f,%d,%d,%d,%d,%d,", size, duration, density, operations[0], operations[1], operations[2], mode, factor);
    fprintf(fptr, "%d,%d,%d,%d,%d\n", p_mb->vertex_insertions, p_mb->vertex_deletions,
      p_mb->edge_insertions, p_mb->edge_deletions, p_mb->edge_searches);

    fclose(fptr);
  } else {
    printf("Duration           : %d (ms)\n", duration);
    printf("# v insert txs     : %d (%f / s)\n", p_mb->vertex_insertions, p_mb->vertex_insertions * 1000.0 / duration);
    printf("# v del txs        : %d (%f / s)\n", p_mb->vertex_deletions, p_mb->vertex_deletions * 1000.0 / duration);
    printf("# e inser txs      : %d (%f / s)\n", p_mb->edge_insertions, p_mb->edge_insertions * 1000.0 / duration);
    printf("# e deltxs         : %d (%f / s)\n", p_mb->edge_deletions, p_mb->edge_deletions * 1000.0 / duration);
    printf("# e searchtxs      : %d (%f / s)\n", p_mb->edge_searches, p_mb->edge_searches * 1000.0 / duration); 
  }

  destruct_graph(p_mb->p_g);
  free(p_mb->op_array);
  free(p_mb);

  return 0;
}