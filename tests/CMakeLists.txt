add_executable(test_add_vertex test_add_vertex.c)
target_link_libraries(test_add_vertex PRIVATE graph graphDao)

add_executable(test_remove_vertex test_remove_vertex.c)
target_link_libraries(test_remove_vertex PRIVATE graph graphDao)

add_executable(test_add_edge test_add_edge.c)
target_link_libraries(test_add_edge PRIVATE graph graphDao)

add_executable(test_remove_edge test_remove_edge.c)
target_link_libraries(test_remove_edge PRIVATE graph graphDao)

add_executable(test_neighborhood test_neighborhood.c)
target_link_libraries(test_neighborhood PRIVATE graph graphDao)

add_test(test_add_vertex test_add_vertex)
add_test(test_remove_vertex test_remove_vertex)
add_test(test_add_edge test_add_edge)
add_test(test_remove_edge test_remove_edge)
add_test(test_neighborhood test_neighborhood)
