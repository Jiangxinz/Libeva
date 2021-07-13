#include <iostream>
#include <stdio.h>
#include <unistd.h>  // usleep
#include "libeva/Thread/MyThreadPool.h"

void print()
{
  // printf("tid=%p\n", std::this_thread::get_id());
	std::cout << "tid=" << std::this_thread::get_id() << std::endl;
}

void printString(const std::string& str)
{
	std::cout << "tid=" << std::this_thread::get_id() << " ";
  printf("%s\n", str.c_str());
  usleep(100*1000);
}

int
main( int argc, char *argv[] ) {
	libeva::MyThreadPool pool;
  	pool.start(5);

	pool.run(print);
	pool.run(print);
	for ( int i = 0; i < 100; ++i ) {
		char buf[32] = {0};
		snprintf(buf, sizeof buf, "task %d", i);
		pool.run( std::bind( printString, std::string( buf ) ) );
	}
	usleep( 1000*2000 );
	pool.stop();
}
