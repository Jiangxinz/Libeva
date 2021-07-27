#include "ds/StealingQueue/StealingQueue.h"

#include <unistd.h>
#include <iostream>
#include <thread>
#include <atomic>

std::atomic_bool running = true;
ds::StealingQueue<int> q;

void func1() {
	int val = 0;
	while ( running ) {
		if ( q.tryPopBack( val ) ) {
			printf(" pop back %d \n ", val );
		} else {
			// std::cout << std::this_thread::get_id() << " yield\n";
			std::this_thread::yield();
		}
		sleep( 1 );
	}
}

void func2() {
	int val = 0;
	while ( running ) {
		if ( q.tryStealFront( val ) ) {
			printf(" pop front %d \n ", val );
		} else {
			// std::cout << std::this_thread::get_id() << " yield\n";
			std::this_thread::yield();
		}
		sleep( 1 );
	}
}

void func3() {
	for ( int i = 0; i < 50; ++i ) {
		q.push_back( i );
	}
	sleep( 10 );
	running = false;
	printf("left: [" );
	int val = 0;
	while ( !q.empty() )  {
		printf(" %d ", q.tryStealFront( val ) ? val : -1 );
	}
	printf("]\n");
}




int
main( int argc, char *argv[] ) {
	std::thread t1( func1 );
	std::thread t2( func2 );
	std::thread t3( func3 );
	t1.join();
	t2.join();
	t3.join();

	return 0;
}
