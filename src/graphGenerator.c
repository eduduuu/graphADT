#include "graph.h"
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include "graphGenerator.h"

static inline void rand_init(unsigned short *seed)
{
  seed[0] = (unsigned short)rand();
  seed[1] = (unsigned short)rand();
  seed[2] = (unsigned short)rand();
}

static inline int rand_range(int n, unsigned short *seed) {
  int v = (int)(erand48(seed) * n);
  return v;
}

Graph*
generate_graph(int v, double density) {
	Graph *p_g = create_graph(v);
	int *degrees = calloc(v, sizeof(int));
	int *included = calloc(v, sizeof(int));

	int target_degree = ceil((v-1) * density);
	
	unsigned short seed[3];
	srand((int)time(NULL));
  	rand_init(seed);
	
	for (int i = 0; i < v; i++) {
		int d = degrees[i];

		for (int j = 0; j < v; j++)
			included[j] = 0;

		for (int j = d; j < target_degree; j++) {
			int u = rand_range(v, seed);
			
			int w;
			for (w = u; w < v; w++) {
				if (!included[w] && degrees[w] != target_degree && w != i) {
					u = w;
					break;
				}
			}
			if (w == v) {
				for (w = u; w >= 0; w--) {
					if (!included[w] && degrees[w] != target_degree && w != i) {
						u = w;
						break;
					}
				}
			}
			add_edge(p_g, (Edge){i, u});
			add_edge(p_g, (Edge){u, i});

			included[u] = 1;
			degrees[u]++;
			degrees[i]++;
		}
	}

	return p_g;
}

Graph
*copy_graph(Graph *g) {
	Graph *new_graph = create_graph(get_num_vertices(g));

	for(int i = 0; i < get_num_vertices(g); i++) {
		Neighborhood n = neighbors(g, i);

		while (n.neighbors) {
			for (int j = 0; j < n.blockSize; j++) {
				add_edge(new_graph, (Edge){i, n.neighbors[j]});
				add_edge(new_graph, (Edge){n.neighbors[j], i});
			}
			n = next_neighbors(g, n);
		}
	}

	return new_graph;
}