#include <list>
#include <thread>
#include <mutex>
#define NUM 100000

std::mutex g_mutex;
std::list<int> mylist;

void func1() {
	for ( int i = 0; i < NUM; ++i ) {
		if ( i % 2 ) { 
			std::lock_guard<std::mutex> guard(g_mutex);
			auto it = lower_bound(mylist.begin(), mylist.end(),
					i);
			mylist.insert( it, i );
		}
	}
}

void func2() {
	for ( int i = 0; i < NUM; ++i ) {
		if ( i % 2 == 0 ) { 
			std::lock_guard<std::mutex> guard(g_mutex);
			auto it = lower_bound(mylist.begin(), mylist.end(),
					i);
			mylist.insert( it, i );
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
