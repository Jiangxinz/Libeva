#include "libeva/Thread/MyThreadPool.h"

namespace libeva {

void MyThreadPool::threadRoutine() {
	Task task;
	while ( running_ ) {
		if ( queue_.tryPop( task ) ) {
			task();
		} else {
			std::this_thread::yield();
		}
	}
}

}
