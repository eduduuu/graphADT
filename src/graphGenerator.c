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
generate_graph2(int v, double density) {
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

Graph*
generate_graph(int v, double density, double shuffle_factor) {
	unsigned short seed[3];
	srand((int)time(NULL));
  	rand_init(seed);

	Graph *p_g = create_graph(v);

	int target_degree = ceil((v-1) * density);
	if (target_degree % 2)
		if (target_degree == v-1)
			target_degree--;
		else
			target_degree++;

	int *edges = (int*)malloc(sizeof(int) * target_degree * v);
	int edge_index = 0;

	for (int i = 0; i < v; i++) {
		for (int j = 0; j < target_degree/2; j++) {
			edges[edge_index] = i;
			edges[edge_index+1] = (i+ j + 1) % v;
			edge_index += 2;
		}
	}


	for (int i = 0; i < edge_index; i += 2) {
		add_edge(p_g, (Edge){edges[i], edges[i+1]});
		add_edge(p_g, (Edge){edges[i+1], edges[i]});
	}

	int random_edges = ceil(target_degree * v * shuffle_factor);
	
	for (int i = 0; i < random_edges; i++) {
		int u = rand_range(edge_index / 2, seed);
		int v = rand_range(edge_index / 2, seed);

		if (!has_edge(p_g, (Edge){edges[u*2], edges[v*2+1]}) && !has_edge(p_g, (Edge){edges[v*2], edges[u*2+1]})
			&& edges[u*2] != edges[v*2+1] && edges[v*2] != edges[u*2+1]) {
			erase_edge(p_g, (Edge){edges[u*2], edges[u*2+1]});
			erase_edge(p_g, (Edge){edges[u*2+1], edges[u*2]});

			erase_edge(p_g, (Edge){edges[v*2], edges[v*2+1]});
			erase_edge(p_g, (Edge){edges[v*2+1], edges[v*2]});

			add_edge(p_g, (Edge){edges[u*2], edges[v*2+1]});
			add_edge(p_g, (Edge){edges[v*2+1], edges[u*2]});

			add_edge(p_g, (Edge){edges[v*2], edges[u*2+1]});
			add_edge(p_g, (Edge){edges[u*2+1], edges[v*2]});

			int w = edges[u*2];
			edges[u*2] = edges[v*2];
			edges[v*2] = w;
		}
	}

	free(edges);
	
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