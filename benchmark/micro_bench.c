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
#include <sched.h>
#include <math.h>
// #include <papi.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include "../src/graph.h"

// #define NUM_EVENTS 7
#define NUM_EVENTS 6

/*
const char *EVENT_NAMES[] = {
    "PAPI_TOT_INS",                // Instructions
    "perf::L1-DCACHE-LOADS",       // L1 Loads
    "perf::L1-DCACHE-LOAD-MISSES", // L1 Mi
    "L2_RQSTS:REFERENCES",         // L2 Accesses
    "L2_RQSTS:MISS",
    "perf::LLC-LOADS",    // L3 Accesses
    "perf::LLC-LOAD-MISSES"
};
*/

#define CACHE_CFG(id, op, res) \
    ((uint64_t)(id) | ((uint64_t)(op) << 8) | ((uint64_t)(res) << 16))

static long perf_open(struct perf_event_attr *a, pid_t pid, int cpu, int group, unsigned long flags) {
    return syscall(SYS_perf_event_open, a, pid, cpu, group, flags);
}

static int open_counter(uint32_t type, uint64_t config) {
    struct perf_event_attr a;
    memset(&a, 0, sizeof(a));
    a.size = sizeof(a);
    a.type = type;
    a.config = config;
    a.disabled = 1;
    a.exclude_kernel = 1;
    a.exclude_hv = 1;
    a.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;
    return perf_open(&a, 0, -1, -1, 0);
}

static uint64_t read_counter(int fd) {
    if (fd < 0) return 0;
    struct { uint64_t val, enabled, running; } r = {0};
    if (read(fd, &r, sizeof(r)) != sizeof(r) || r.running == 0) return 0;
    return (uint64_t)((double)r.val * r.enabled / r.running);
}

typedef struct FunctionEvents {
    const char *name;
    long long calls;
    long long events[NUM_EVENTS];
    double scale;
    int kernel_included;
} FunctionEvents;

enum { F_TOTAL, NUM_FUNCS };

FunctionEvents *func_prof[NUM_FUNCS] = {NULL};

static __thread int perf_fds[NUM_EVENTS] = {-1, -1, -1, -1, -1, -1};
static __thread int perf_ready = 0;

static void perf_close_all(void) {
    for (int i = 0; i < NUM_EVENTS; i++) {
        if (perf_fds[i] >= 0) {
            close(perf_fds[i]);
            perf_fds[i] = -1;
        }
    }
    perf_ready = 0;
}

static int perf_init_counters(void) {
    perf_fds[0] = open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);
    perf_fds[1] = open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);
    perf_fds[2] = open_counter(PERF_TYPE_HW_CACHE,
        CACHE_CFG(PERF_COUNT_HW_CACHE_L1D, PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_RESULT_ACCESS));
    perf_fds[3] = open_counter(PERF_TYPE_HW_CACHE,
        CACHE_CFG(PERF_COUNT_HW_CACHE_L1D, PERF_COUNT_HW_CACHE_OP_READ, PERF_COUNT_HW_CACHE_RESULT_MISS));
    perf_fds[4] = open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES);
    perf_fds[5] = open_counter(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES);

    for (int i = 0; i < NUM_EVENTS; i++) {
        if (perf_fds[i] < 0) {
            fprintf(stderr, "warning: counter %d failed to open: %s\n", i, strerror(errno));
        } else {
            ioctl(perf_fds[i], PERF_EVENT_IOC_RESET, 0);
        }
    }
    perf_ready = 1;
    return 1;
}

void profile_start(void) {
    if (perf_ready) {
        for (int i = 0; i < NUM_EVENTS; i++) {
            if (perf_fds[i] >= 0) {
                ioctl(perf_fds[i], PERF_EVENT_IOC_RESET, 0);
            }
        }
        prctl(PR_TASK_PERF_EVENTS_ENABLE);
    }
}

void profile_stop(FunctionEvents *func_prof) {
    if (perf_ready) {
        prctl(PR_TASK_PERF_EVENTS_DISABLE);
        for (int i = 0; i < NUM_EVENTS; i++) {
            if (perf_fds[i] >= 0) {
                func_prof->events[i] += (long long)read_counter(perf_fds[i]);
            }
        }
        func_prof->scale = 1.0;
        func_prof->kernel_included = 0;
    }
    func_prof->calls++;
}




// xorshift64*: a few instructions, no libraries, no setup beyond a non-zero 64-bit state
static inline uint64_t rng_next(uint64_t *s)
{
  uint64_t x = *s;
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  *s = x;
  return x * 0x2545F4914F6CDD1DULL;
}

static void rng_seed(uint64_t *s)
{
  // splitmix64 over time and pid so the state is never zero
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  uint64_t z = (uint64_t)ts.tv_nsec ^ ((uint64_t)ts.tv_sec << 32) ^ ((uint64_t)getpid() << 16);
  z += 0x9E3779B97F4A7C15ULL;
  z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
  z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
  z ^= z >> 31;
  *s = z ? z : 0x9E3779B97F4A7C15ULL;
}

// Uniform integer in [0, n) using the high 32 bits (multiply-shift, no division)
static inline int rand_range(int n, uint64_t *seed) {
  int v = (int)(((rng_next(seed) >> 32) * (uint64_t)n) >> 32);
  //assert (v >= 0 && v < n);
  return v;
}

typedef struct micro_bech {
  Graph *p_g;
  int *op_array;
  long long vertex_insertions;
  long long vertex_deletions;
  long long edge_insertions;
  long long edge_deletions;
  long long edge_searches;
  int op_count;
  int duration;
  int op_distribution[3];
  uint64_t seed;
} micro_bench;

static volatile int tid;
static volatile int stop;
static void*
test_random(void *p_mb_) {
  tid = gettid();
  printf("tid %d\n", tid);
  fflush(stdout);

  /*
  // Register thread and create EventSet for THIS worker thread
  PAPI_register_thread();

  int ret = PAPI_create_eventset(&EventSet);
  if (ret != PAPI_OK) {
    fprintf(stderr, "Thread PAPI create eventset error: %s\n", PAPI_strerror(ret));
  } else {
    PAPI_set_multiplex(EventSet);
    for (int i = 0; i < NUM_EVENTS; i++) {
      ret = PAPI_add_named_event(EventSet, EVENT_NAMES[i]);
      if (ret != PAPI_OK) {
        fprintf(stderr, "Thread PAPI add event error: %s (%s)\n", EVENT_NAMES[i], PAPI_strerror(ret));
      }
    }
  }
  */

  // Pin the worker to the CPU it is currently running on: no migrations during the run
  {
    int cpu = sched_getcpu();
    if (cpu >= 0) {
      cpu_set_t set;
      CPU_ZERO(&set);
      CPU_SET(cpu, &set);
      if (sched_setaffinity(0, sizeof(set), &set) != 0)
        perror("sched_setaffinity");
    }
  }

  perf_ready = perf_init_counters();
  if (!perf_ready)
    fprintf(stderr, "warning: perf counters unavailable, hardware events will be 0\n");

  micro_bench *p_mb = (micro_bench*)p_mb_;
  Graph *g = p_mb->p_g;
  uint64_t rng = p_mb->seed;
  const int thr_update = p_mb->op_distribution[1];
  const int thr_search = p_mb->op_distribution[2];
  // Vertex updates are not benchmarked, so the vertex count never changes
  const int n = get_num_vertices(g);
  int op, u, v;

  // 64-bit local counters (no int overflow, no memory traffic through p_mb in the loop)
  long long ins = 0, del = 0, srch = 0;

  // Edge updates come in pairs on the same edge, so the graph density stays constant:
  // pick a random edge; if it is in the graph remove it, otherwise add it. The next
  // edge update undoes that (re-adds / removes the same edge). Then a new edge is picked.
  int pending = 0, pending_add = 0;
  Edge pending_edge = {0, 0};

  while (stop == -1) {
    sched_yield();
  }

  // Single outer profile: counters cover the whole measured run (no per-call overhead)
  profile_start();
  while (stop == 0) {
    op = rand_range(100, &rng);
    // Vertex updates are intentionally NOT benchmarked (the implementations still
    // provide add_vertex/erase_vertex). main() enforces vertex update share == 0,
    // so op_distribution[0] is always 0 and only edge operations are measured.

    // edge update
    if (op < thr_update) {
      if (pending) {
        if (pending_add) {
          add_edge(g, pending_edge);
          ins++;
        } else {
          erase_edge(g, pending_edge);
          del++;
        }
        pending = 0;
      } else {
        u = rand_range(n, &rng);
        v = rand_range(n, &rng);
        pending_edge = (Edge){u, v};
        if (has_edge(g, pending_edge)) {
          erase_edge(g, pending_edge);
          del++;
          pending_add = 1;   // next edge update re-adds it
        } else {
          add_edge(g, pending_edge);
          ins++;
          pending_add = 0;   // next edge update removes it again
        }
        pending = 1;
      }
    // edge search
    } else if (op < thr_search) {
      u = rand_range(n, &rng);
      v = rand_range(n, &rng);
      has_edge(g, (Edge){u, v});
      srch++;
    }
  }
  profile_stop(func_prof[F_TOTAL]);

  p_mb->edge_insertions += ins;
  p_mb->edge_deletions += del;
  p_mb->edge_searches += srch;
  p_mb->seed = rng;

  /*
  if (EventSet != PAPI_NULL) {
    PAPI_cleanup_eventset(EventSet);
    PAPI_destroy_eventset(&EventSet);
    EventSet = PAPI_NULL;
  }
  PAPI_unregister_thread();
  */
  perf_close_all();
  return NULL;
}

void run_test(micro_bench *p_mb, void*(test_func)(void*)) {
  stop = -1;
  tid = -1;

  struct timeval start, end;
  struct timespec timeout;

  timeout.tv_sec = p_mb->duration / 1000;
  timeout.tv_nsec = (p_mb->duration % 1000) * 1000000;

  /*
  if (!PAPI_is_initialized()) {
    int ret = PAPI_library_init(PAPI_VER_CURRENT);
    if (ret != PAPI_VER_CURRENT) {
      fprintf(stderr, "PAPI library init error!\n");
      exit(1);
    }
    ret = PAPI_thread_init(pthread_self);
    if (ret != PAPI_OK) {
      fprintf(stderr, "PAPI thread init error: %s\n", PAPI_strerror(ret));
    }
    ret = PAPI_multiplex_init();
    if (ret != PAPI_OK) {
      fprintf(stderr, "PAPI multiplex init error: %s\n", PAPI_strerror(ret));
    }
  }
  */

  for (int i = 0; i < NUM_FUNCS; i++) {
    if (func_prof[i]) {
      free(func_prof[i]);
    }
    func_prof[i] = (FunctionEvents*)calloc(1, sizeof(FunctionEvents));
  }
  func_prof[F_TOTAL]->name = "total";


  pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t));
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_create(thread, &attr, test_func, (void*)p_mb);
  pthread_setname_np(*thread, "bench_thread");

  getchar();
  stop = 0;

  gettimeofday(&start, NULL);
  nanosleep(&timeout, NULL);

  stop = 1;

  gettimeofday(&end, NULL);
  pthread_join(*thread, NULL);
  free(thread);

  p_mb->duration = (end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);
}

int choose_vertex(Graph *g, uint64_t *seed) {
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

void choose_edge(Graph *g, uint64_t *seed, int has, Edge* edge) {
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
  return NULL;
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
    op = rand_range(100, &p_mb->seed);
    // vertex update
    if (op < p_mb->op_distribution[0]) {
      if (!vertex_update)
        continue;

      u = rand_range(get_num_vertices(g), &p_mb->seed);
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

      choose_edge(g, &p_mb->seed, insert_edge, &edge);
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

      choose_edge(g, &p_mb->seed, 1, &edge);

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
  return p_mb->op_array;
}
/**
 * Main Function to execute the micro bench.
 *
 * can receive the following arguments when the program is executed:
 * t = test duration
 *   s = graph size
 *  d = graph density
 *  a = vertex update
 *  b = edge update
 *  c = edge search
 *  m = type of test
 *  f = factor
 *  w = file to write
 *  g = factor to shuffle edges on generator
 */
int main(int argc, char **argv) {

  // Default values: 0% vertex (disabled), 20% edge updates, 80% edge searches
  int duration = 1000;
  int size = 100;
  double density = 0.75;
  int operations[] = {0, 20, 80};
  int mode = 0;
  int factor = 1;
  double shuffle_factor = 0;
  char file_name[50] = "micro_benchmark.csv";
  char implementation[30];
  implementation[0] = '\0';
  char workload[30];
  workload[0] = '\0';

  // Set variables accordingly to options from the command line
  char opt;
  while ((opt = getopt(argc, argv, "t:s:d:a:b:c:m:f:w:g:i:k:")) != -1) {
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
        // Vertex operations are removed from benchmark; force to 0
        operations[0] = 0;
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
        snprintf(file_name, sizeof(file_name), "%s", optarg);
        break;
      case 'g':
        shuffle_factor = atoi(optarg);
        break;
      case 'i':
        snprintf(implementation, sizeof(implementation), "%s", optarg);
        break;
      case 'k':
        snprintf(workload, sizeof(workload), "%s", optarg);
        break;
    }
  }
  operations[0] = 0;
  int pct[3] = {operations[0], operations[1], operations[2]};
  operations[1] = operations[0] + operations[1];
  operations[2] = operations[1] + operations[2];

  // Assert valid option values
  assert(operations[2] == 100);
  assert(size >= 1);
  assert(density > 0 && density <= 1);
  assert(mode == 0 || mode == 1);
  assert(factor >= 0);
  assert(shuffle_factor >= 0);

  // New bench
  micro_bench *p_mb = (micro_bench*)calloc(1, sizeof(micro_bench));
  p_mb->p_g = generate_graph(size, density, shuffle_factor);

  printf("Generated Graph\n");
  fflush(stdout);

  //p_mb->p_g = create_graph(size);
  srand((int)time(NULL));
  rng_seed(&p_mb->seed);

  p_mb->vertex_insertions = 0;
  p_mb->vertex_deletions = 0;
  p_mb->edge_insertions = 0;
  p_mb->edge_deletions = 0;
  p_mb->edge_searches = 0;
  p_mb->duration = duration;

  p_mb->op_distribution[0] = operations[0];
  p_mb->op_distribution[1] = operations[1];
  p_mb->op_distribution[2] = operations[2];

  // Run bench
  if (mode) {
    printf("generating operations\n");
    fflush(stdout);
    generate_operations(p_mb);

    printf("Running test\n");
    fflush(stdout);
    run_test(p_mb, test);
  } else {
    printf("Running test\n");
    fflush(stdout);
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

  // Use the measured duration (ms) instead of the requested one
  duration = p_mb->duration;

  // Output to file or cout
  if (file_name[0] != '\0') {
    printf("Adding to file");
    fflush(stdout);
    int insert_header = access(file_name, F_OK);
    FILE *fptr = fopen(file_name, "a");

    if (!fptr) {
      perror("fopen results csv");
    } else {
      if (insert_header) {
        printf("Inserting Header");
        fflush(stdout);
        fprintf(fptr, "size,duration,density,implementation,workload,v_updates,e_updates,e_searches,mode,factor,v_insert,v_del,e_insert,e_del,e_searches_done\n");
      }

      // v_updates/e_updates/e_searches are the percentages given on the command line
      fprintf(fptr, "%d,%d,%f,%s,%s,%d,%d,%d,%d,%d,", size, duration, density, implementation, workload, pct[0], pct[1], pct[2], mode, factor);
      fprintf(fptr, "%lld,%lld,%lld,%lld,%lld\n", p_mb->vertex_insertions, p_mb->vertex_deletions,
        p_mb->edge_insertions, p_mb->edge_deletions, p_mb->edge_searches);

      fclose(fptr);
    }
  } else {
    printf("Duration           : %d (ms)\n", duration);
    printf("# v insert txs     : %lld (%f / s)\n", p_mb->vertex_insertions, p_mb->vertex_insertions * 1000.0 / duration);
    printf("# v del txs        : %lld (%f / s)\n", p_mb->vertex_deletions, p_mb->vertex_deletions * 1000.0 / duration);
    printf("# e inser txs      : %lld (%f / s)\n", p_mb->edge_insertions, p_mb->edge_insertions * 1000.0 / duration);
    printf("# e deltxs         : %lld (%f / s)\n", p_mb->edge_deletions, p_mb->edge_deletions * 1000.0 / duration);
    printf("# e searchtxs      : %lld (%f / s)\n", p_mb->edge_searches, p_mb->edge_searches * 1000.0 / duration);
    fflush(stdout);
  }

  const char *prof_file = "benchmark_cache_per_function_total.csv";
  int insert_prof_header = access(prof_file, F_OK);
  FILE *fptr = fopen(prof_file, "a");
  if (!fptr) {
    perror("fopen profile csv");
  } else if (insert_prof_header) {
    fprintf(fptr, "size,duration,density,implementation,workload,function,calls,cycles,instructions,l1_dcache_loads,l1_dcache_load_misses,llc_loads,llc_load_misses,perf_scale,kernel_included\n");
  }

  for (int i = 0; i < NUM_FUNCS; i++) {
    if (fptr) {
      fprintf(fptr, "%d,%d,%f,%s,%s,%s,%lld", size, duration, density, implementation, workload, func_prof[i]->name, func_prof[i]->calls);
      for (int j = 0; j < NUM_EVENTS; j++) {
        fprintf(fptr, ",%lld", func_prof[i]->events[j]);
      }
      fprintf(fptr, ",%.4f,%d\n", func_prof[i]->scale, func_prof[i]->kernel_included);
    }
    free(func_prof[i]);
    func_prof[i] = NULL;
  }


  if (fptr) fclose(fptr);



  destruct_graph(p_mb->p_g);
  free(p_mb->op_array);
  free(p_mb);


  return 0;
}
