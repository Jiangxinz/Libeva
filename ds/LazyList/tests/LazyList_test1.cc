#include "ds/LazyList/LazyList.h"
#include "utility/SpinLock.h"

using namespace ds;

int
main(int argc, char *argv[]) {
	LazyList<int> mylist( INT_MAX );
	for ( int i = 0; i < 1000; ++i ) {
		mylist.add( i );
	}
	for ( int i = 0; i < 1000; ++i ) {
		if ( i % 2 == 0 ) {
			mylist.remove( i );
		}
	}

	mylist.traversal( [&]( const int x ) {
			printf("%d ", x);
			} );
	printf("\n");
	return 0;
}
