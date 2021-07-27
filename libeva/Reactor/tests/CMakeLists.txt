add_executable(test1 test1.cc)
target_link_libraries(test1 eva_base)

add_executable(test2 test2.cc)
target_link_libraries(test2 eva_base)

add_executable(test3 test3.cc)
target_link_libraries(test3 eva_base)

add_executable(test4 test4.cc)
target_link_libraries(test4 eva_base)

add_executable(test5 test5.cc)
target_link_libraries(test5 eva_base)

add_executable(reactorthread_test ReactorThread_test.cc)
target_link_libraries(reactorthread_test eva_base)

add_executable(reactorthreadpool_unittest ReactorThreadPool_unittest.cc)
target_link_libraries(reactorthreadpool_unittest eva_base)
