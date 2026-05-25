cmake_minimum_required(VERSION 3.31)

project(GraphADT C)

set(IMPLEMENTATION "ADJACENCY_LIST" CACHE STRING "Graph Implementation")

set(GRAPH_IMPLEMENTATION "")
if(IMPLEMENTATION STREQUAL "CSR")
  set(GRAPH_IMPLEMENTATION src/csr.c)
elseif(IMPLEMENTATION STREQUAL "ADJACENCY_MATRIX")
  set(GRAPH_IMPLEMENTATION src/adjacencyMatrix.c)
elseif(IMPLEMENTATION STREQUAL "ADJACENCY_LIST")
  set(GRAPH_IMPLEMENTATION src/adjacencyList.c)
endif()

message("${GRAPH_IMPLEMENTATION}")

add_executable(Main)
target_sources(Main
  PRIVATE
    src/main.c
)

add_library(graph STATIC ${GRAPH_IMPLEMENTATION} src/assert.c)
target_sources(graph
  PUBLIC
    FILE_SET graph
    TYPE HEADERS
    FILES
      src/graph.h
)

add_library(graphDao STATIC src/graphDao.c)
target_sources(graphDao
  PUBLIC
    FILE_SET graphDao
    TYPE HEADERS
    FILES
      src/graphDao.h
)
target_link_libraries(Main
  PRIVATE
    graph
    graphDao
)

enable_testing()
include(CTest)
include_directories(src)
add_subdirectory(tests)
add_subdirectory(benchmark)
