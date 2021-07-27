set(base_SRCS
	# AsyncLogging.cc
  	# Thread/Condition.cc
  	Thread/CountDownLatch.cc
  	Thread/CurrentThread.cc
	Thread/Thread.cc
	Thread/MyThreadPool.cc
	# logger/asynclogging.cc
	# logger/fileutil.cc
	# logger/logfile.cc
	# logger/logging.cc
	# logger/logstream.cc
	Logger/LogFile.cc
	Logger/Logging.cc
	Logger/LogStream.cc
	Logger/FileUtil.cc
  	# Date.cc
  	# Exception.cc
  	# FileUtil.cc
  	# Logger/LogFile.cc
  	# Logger/Logging.cc
  	# Logger/LogStream.cc
  	# ProcessInfo.cc
  	Timestamp.cc
  	# Thread.cc
  	# ThreadPool.cc
  	# TimeZone.cc
	Reactor/Reactor.cc
	Reactor/Demultiplexer.cc
	Reactor/Event.cc
	Reactor/ReactorThread.cc
	Reactor/ReactorThreadPool.cc
	TimeWheel/TimeWheel.cc
	TimeWheel/Timer.cc
	Net/Acceptor.cc
	Net/Socket.cc
	Net/InetAddr.cc
	Net/TcpConnection.cc
	Net/TcpServer.cc
	Net/Buffer.cc
  )

add_library(eva_base ${base_SRCS})
target_link_libraries(eva_base pthread rt)

#add_library(muduo_base_cpp11 ${base_SRCS})
#target_link_libraries(muduo_base_cpp11 pthread rt)
#set_target_properties(muduo_base_cpp11 PROPERTIES COMPILE_FLAGS "-std=c++0x")

install(TARGETS eva_base DESTINATION lib)
#install(TARGETS muduo_base_cpp11 DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/libeva/base)

# if(MUDUO_BUILD_EXAMPLES)
#  add_subdirectory(tests)
# endif()

add_subdirectory(Reactor/tests)
add_subdirectory(Thread/tests)
add_subdirectory(Logger/tests)
add_subdirectory(TimeWheel/tests)
add_subdirectory(Net/tests)
add_subdirectory(examples)
