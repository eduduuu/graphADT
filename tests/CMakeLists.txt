add_executable(test1 test.c)
target_link_libraries(test1 PRIVATE graph graphDao)

add_test(test_1 test1)
