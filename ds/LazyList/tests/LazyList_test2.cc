#include <thread>
#include <time.h>
#include "ds/LazyList/LazyList.h"
#include "utility/SpinLock.h"
#define NUM 100000

using namespace ds;

LazyList<int> mylist( INT_MAX );

void func1() {
	for ( int i = 0; i < NUM; ++i ) {
		if ( i % 2 ) { 
			mylist.add( i );
		}
	}
}

void func2() {
	for ( int i = 0; i < NUM; ++i ) {
		if ( i % 2 == 0 ) { 
			mylist.add( i );
		}
	}
}

int
main(int argc, char *argv[]) {
	time_t start = time(NULL);
	std::thread t1( func1 );
	std::thread t2( func2 );
	t1.join();
	t2.join();
	time_t end = time(NULL);
	printf("%ld\n", end-start);
	return 0;
}
