#include <stdio.h>
#include "graph.h"
#include "graphDao.h"

int main() {
   Graph *g = create_graph(3);
   add_edge(g, (Edge){0, 1});
   add_edge(g, (Edge){0, 2});
   add_edge(g, (Edge){1, 2});
   print_graph(g);
   destruct_graph(g);
   return 0;
}
/*
 int main() {
 Graph *p_graph = read_graph("email-Enron.txt");
 //print_graph(p_graph);
 destruct_graph(p_graph);
}
*/
