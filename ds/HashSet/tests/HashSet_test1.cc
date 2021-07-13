#include "ds/HashSet/HashSet.h"

int
main(int argc, char *argv[]) {
	ds::HashSet<int> hashSet( INT_MAX );
	for ( int i = 0; i < 1000; ++i ) {
		bool ret = hashSet.add( i );
		assert( ret ); (void) ret;
	}
	for ( int i = 0; i < 1000; ++i ) {
		bool ret = hashSet.contains( i );
		assert ( ret ); (void) ret;
		ret = hashSet.remove( i );
		assert( ret ); (void) ret;
	}
}
