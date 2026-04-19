This repo has the code for a research on overhead analysis in graph data structures.

The file src/graph.h has the functions for the graph interface.

For compiling, you can execute the following commands after cloning the code:
```
cmake -B build
cmake --build build
```
You can link with different implementations seting the flag in the cmake configuration command. The possible implementations are ADJACENCY_LIST and ADJACENCY_MATRIX.
If you want to use adjacency lists for example, run:
```
cmake -B build -DADJACENCY_LIST=ON
cmake --build build
``` 
To test, you can run:
```
./build/test1
```
