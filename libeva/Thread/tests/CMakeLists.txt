add_executable(emutex_test mutex_test.cc)
target_link_libraries(emutex_test eva_base)

add_executable(ethread_test thread_test.cc)
target_link_libraries(ethread_test eva_base)

add_executable(ethread_bench thread_bench.cc)
target_link_libraries(ethread_bench eva_base)

add_executable(mythreadpool_test1 mythreadpool_test1.cc)
target_link_libraries(mythreadpool_test1 eva_base)
