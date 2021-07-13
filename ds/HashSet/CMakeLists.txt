add_executable(hashset_test1 tests/HashSet_test1.cc)

add_executable(hashset_test2 tests/HashSet_test2.cc)
target_link_libraries(hashset_test2 pthread)
