#include "graph.h"
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

	free(degrees);
	free(included);

	return p_g;
}

Graph*
generate_graph(int v, double density, double shuffle_factor) {
	(void)shuffle_factor; // random rewiring is not implemented (kept for API compatibility)

	Graph *p_g = create_graph(v);

	// Directed ring lattice: every vertex gets `target_degree` out-neighbours
	// i+1, ..., i+target_degree (mod v). Offsets are distinct and never 0, so
	// there are no self-loops or duplicates.
	int target_degree = (int)ceil((v-1) * density);
	if (target_degree > v-1)
		target_degree = v-1;
	if (target_degree <= 0)
		return p_g;

	size_t total = (size_t)target_degree * v;
	Edge *edges = (Edge*)malloc(sizeof(Edge) * total);
	if (!edges) {
		perror("generate_graph: malloc");
		exit(1);
	}

	size_t edge_index = 0;
	for (int i = 0; i < v; i++) {
		for (int j = 0; j < target_degree; j++) {
			edges[edge_index] = (Edge){ i, (i + j + 1) % v };
			edge_index += 1;
		}
	}

	add_edges(p_g, edges, edge_index);
	free(edges);

	return p_g;
}

Graph
*copy_graph(Graph *g) {
	int n = get_num_vertices(g);
	Graph *new_graph = create_graph(n);

	size_t capacity = 1024, count = 0;
	Edge *edges = (Edge*)malloc(sizeof(Edge) * capacity);

	for (int i = 0; i < n; i++) {
		Neighborhood nb = neighbors(g, i);

		while (nb.neighbors) {
			for (size_t j = 0; j < nb.blockSize; j++) {
				if (count == capacity) {
					capacity *= 2;
					edges = (Edge*)realloc(edges, sizeof(Edge) * capacity);
				}
				edges[count++] = (Edge){i, nb.neighbors[j]};
			}
			nb = next_neighbors(g, nb);
		}
	}

	// Edges are copied exactly as they are (the graph is directed)
	add_edges(new_graph, edges, count);
	free(edges);

	// Preserve removed vertices
	for (int i = 0; i < n; i++)
		if (!has_vertex(g, i))
			erase_vertex(new_graph, i);

	return new_graph;
}
