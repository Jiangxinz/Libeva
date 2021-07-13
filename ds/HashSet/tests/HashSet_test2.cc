#include <thread>
#include "ds/HashSet/HashSet.h"


ds::HashSet<int> hashSet( INT_MAX );

void func1() {
	for ( int i = 0; i < 1000; ++i ) {
		if ( i % 2 == 0 ) {
			bool ret = hashSet.add( i );
			assert( ret ); (void) ret;
		}
	}
}

void func2() {
	for ( int i = 0; i < 1000; ++i ) {
		if ( i % 2 ) {
			bool ret = hashSet.add( i );
			assert( ret ); (void) ret;
		}
	}
}

int
main(int argc, char *argv[]) {
	std::thread t1( func1 );
	std::thread t2( func2 );

	t1.join();
	t2.join();

	for ( int i = 0; i < 1000; ++i ) {
		bool ret = hashSet.contains( i );
		assert ( ret ); (void) ret;
		ret = hashSet.remove( i );
		assert( ret ); (void) ret;
	}
}
