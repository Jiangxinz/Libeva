#include <thread>
#include "ds/FineGrainedQueue/FineGrainedQueue.h"
// NUM >= 2
#define NUM 200000


class Data {
public:
	Data() : val1_( 0 ) {}
	
	explicit Data( int val ) :
		val1_( val ) {
			// printf("Data( int val )\n");
		}
	Data( Data &&other ) noexcept {
		// printf("Data( Data &&other )\n");
		val1_ = other.val1_;
		other.val1_ = -1;
	}

	Data& operator=( Data &&other ) noexcept {
		// printf("Data& operator=( Data &&other)\n");
		val1_ = other.val1_;
		other.val1_ = -1;
		return *this;
	}
	~Data() {
		// printf("~Data(), val1_=%d\n", val1_);
	}


	static std::shared_ptr<Data> makeData() {
		return std::make_shared<Data>();
	}
	int val1_;
};

ds::FineGrainedQueue<Data> q;

void func1() {
	for ( int i = 0; i< NUM; ++i ) {
		if ( i % 2 == 0 ) {
			q.push( Data(i) );
		}
	}
	printf("func1 finished\n");
}

void func2() {
	for ( int i = 0; i< NUM; ++i ) {
		if ( i % 2 ) {
			q.push( Data(i) );
		}
	}
	printf("func2 finished\n");
}

void func3() {
	bool fin1 = false, fin2 = false;
	while( !fin1 && !fin2 ) {
		Data ret = q.waitAndPop();
		if( ret.val1_ == NUM-1 ) {
			fin1 = true;
		}
		if ( ret.val1_ == NUM-2 ) {
			fin2 = true;
		}
	}
	printf("func3 finished\n");

}

int
main(int argc, char *argv[]) {
	std::thread t1( func1 );
	std::thread t2( func2 );
	std::thread t3( func3 );
	t1.join();
	t2.join();
	t3.join();

	return 0;
}
