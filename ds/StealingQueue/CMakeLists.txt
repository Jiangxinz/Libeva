add_executable( stealingqueue_test1 tests/StealingQueue_test1.cc )
target_link_libraries( stealingqueue_test1 pthread )

add_executable( stealingqueue_test2 tests/StealingQueue_test2.cc )
target_link_libraries( stealingqueue_test2 pthread )
