set(base_SRCS
	MiniMemcached.cc
	MiniMemcached_debug.cc
	Conversation.cc
	Entry.cc
	)

add_executable(minimemcached_debug ${base_SRCS})
target_link_libraries(minimemcached_debug eva_base)

set(base_SRCS2
	MiniMemcached.cc
	MiniMemcached_debug_pprof.cc
	Conversation.cc
	Entry.cc
	)

add_executable(minimemcached_debug_pprof ${base_SRCS2})
target_link_libraries(minimemcached_debug_pprof eva_base tcmalloc profiler)
