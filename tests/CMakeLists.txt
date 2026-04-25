add_executable(test_add_vertex test_add_vertex.c)
target_link_libraries(test_add_vertex PRIVATE graph graphDao)

add_test(test_add_vertex1 test_add_vertex)
