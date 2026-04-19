#include <stdio.h>
#include "../src/graph.h"
#include "../src/graphDao.h"

int main() {
    printf("initing test");
   Graph *g = create_graph(3);
   add_edge(g, (Edge){0, 1});
   if (!has_edge(g, (Edge){0, 1})) {
     destruct_graph(g);
     return 1;
   }
   add_edge(g, (Edge){0, 2});
   if (!has_edge(g, (Edge){0, 2})) {
     destruct_graph(g);
     return 1;
   }
   
   Neighborhood n = neighbors(g, 0);
   printf("%d", n.neighbors[0]);
   if (n.neighbors[0] != 2) {
     destruct_graph(g);
     return 1;
   }
  n = next_neighbors(g, n);
  if (n.neighbors[0] != 1) {
     destruct_graph(g);
     return 1;
   }
   if (!has_vertex(g, 1)) {
     destruct_graph(g);
     return 1;
   }

   add_edge(g, (Edge){1, 2});
   if (!has_edge(g, (Edge){1, 2})){
     destruct_graph(g);
     return 1;
   }
   add_vertex(g, 4);
   if (!has_vertex(g, 4)){
     destruct_graph(g);
     return 1;
   }
   add_edge(g, (Edge){4, 2});
   if (!has_edge(g, (Edge){4, 2})){
     destruct_graph(g);
     return 1;
   }
   erase_vertex(g, 2);
   if (has_vertex(g, 2)){
     destruct_graph(g);
     return 1;
   }
   erase_edge(g, (Edge){0, 1});
   if (has_edge(g, (Edge){0, 1})){
     destruct_graph(g);
     return 1;
   }
  destruct_graph(g);
   //print_graph(g);
    printf("Passed 1");
   return 0;
}
/*
 int main() {
 Graph *p_graph = read_graph("email-Enron.txt");
 //print_graph(p_graph);
 destruct_graph(p_graph);
}
*/
