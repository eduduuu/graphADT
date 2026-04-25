cmake_minimum_required(VERSION 3.31)

project(GraphADT C)

option(ADJACENCY_LIST "Use Adjacency List" OFF)
option(ADJACENCY_MATRIX "Use Adjacency Matrix" OFF)

set(GRAPH_IMPLEMENTATION "")
if(ADJACENCY_LIST)
  set(GRAPH_IMPLEMENTATION src/adjacencyList.c)
elseif(ADJACENCY_MATRIX)
  set(GRAPH_IMPLEMENTATION src/adjacencyMatrix.c)
else()
  set(GRAPH_IMPLEMENTATION src/adjacencyList.c)
endif()

add_executable(Main)
target_sources(Main
  PRIVATE
    src/main.c
)
message("${GRAPH_IMPLEMENTATION}")
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
