#ifndef _LIBEVA_THREAD_THREADPOOL_H_
#define _LIBEVA_THREAD_THREADPOOL_H_

#include <atomic>
#include <vector>
#include <thread>
#include <functional>
#include "../noncopyable.h"
#include "ds/FineGrainedQueue/FineGrainedQueue.h"
#include "libeva/Thread/ThreadsJoiner.h"
#include "libeva/Thread/CountDownLatch.h"

namespace libeva {


class MyThreadPool : libeva::noncopyable {
	typedef std::function<void(void)> Task;
public:
	MyThreadPool() :
		running_( false ),
		queue_(),
		threads_(), 
		joiner_( threads_ )
       		{ }

	void start( int numThreads ) {
		running_ = true;
		threads_.resize( numThreads );
		try {
			for ( int i = 0; i < numThreads; ++i ) {
				threads_[ i ] = std::thread( 
						std::bind( &MyThreadPool::threadRoutine, this )
						);
			}
		} catch ( ... ) {
			running_ = false;
			throw;
		}
	}
	void stop() {
		running_ = false;
	}

	void run( Task task ) {
		queue_.push( std::move( task ) );
	}

	~MyThreadPool() {
		running_ = false;
	}

private:
	void threadRoutine();

	std::atomic_bool running_;
	ds::FineGrainedQueue<Task> queue_;
	std::vector<std::thread> threads_;
	ThreadsJoiner<std::vector<std::thread>> joiner_;
};

}


#endif /* _LIBEVA_THREAD_THREADPOOL_H_ */
