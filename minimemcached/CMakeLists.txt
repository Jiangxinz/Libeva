set(base_SRCS
	MiniMemcached.cc
	MiniMemcached_debug.cc
	Conversation.cc
	Entry.cc
	# utility/Tokenizer.cc
	)

add_executable(minimemcached_debug ${base_SRCS})
target_link_libraries(minimemcached_debug eva_base)

