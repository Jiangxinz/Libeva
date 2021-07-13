#include <iostream>
#include <list>
#define NUM 100000

using namespace std;

int
main(int argc, char *argv[]) {
	list<int> stdList;
	time_t start = time(NULL);
	for ( int i = 0; i < NUM; ++i ) {
		auto it = lower_bound( stdList.begin(), stdList.end(), i );
		stdList.insert( it, i );
	}
	time_t end = time(NULL);
	printf("%ld\n", end-start);
	return 0;
}
