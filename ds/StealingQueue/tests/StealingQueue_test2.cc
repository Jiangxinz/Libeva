#include "ds/StealingQueue/StealingQueue.h"

#include <unistd.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>

std::atomic_bool running = true;
typedef std::function<void(void)> WorkFunc;
ds::StealingQueue<WorkFunc> q;

void func() {
	std::cout << std::this_thread::get_id() << " pop something\n";
	usleep( 1000 * 10 );
}

void func1() {
	WorkFunc func;
	while ( running ) {
		if ( q.tryPopBack( func ) ) {
			func();
		}
		sleep( 1 );
	}
}

void func2() {
	WorkFunc func;
	while ( running ) {
		if ( q.tryStealFront( func ) ) {
			func();
		}
		sleep( 1 );
	}
}

void finish() {
	running = false;
}

void func3() {
	for ( int i = 0; i < 50; ++i ) {
		q.push_back( func );
	}
	sleep( 10 );
	q.push_back( finish );
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
