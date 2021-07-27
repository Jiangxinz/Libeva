#include "libeva/Thread/MyThreadPool.h"

namespace libeva {


thread_local ds::StealingQueue<MyThreadPool::Task>* MyThreadPool::private_queue_ = nullptr;
thread_local int MyThreadPool::idx_ = 0;

MyThreadPool::MyThreadPool() :
	numThreads_( 0 ),
	running_( false ),
	shared_queue_(),
	queues_(),
	threads_(), 
	joiner_( threads_ )
       	{ }

void MyThreadPool::threadRoutine( unsigned idx ) {
	idx_ = idx;
	private_queue_ = queues_.at( idx ).get();
	Task task;
	while ( running_ ) {
		if ( private_queue_ && private_queue_->tryPopBack( task ) ) {
			printf("pop from private queue: ");
			task();
		} else if ( shared_queue_.tryPop( task ) ) {
			printf("pop from shared queue: ");
			task();
		} else if ( stealWork( task ) ) {
			printf("stealing from other queue: ");
			task();
		} else {
			std::this_thread::yield();
		}
	}
}

bool MyThreadPool::stealWork( Task &task ) {
	int numQueue = static_cast<int>( queues_.size() );
	bool ret = false;
	for ( int i = 0; i < numQueue; ++i ) {
		const int index = ( idx_+i+1 ) % numQueue;
		if ( queues_.at( index )->tryStealFront( task ) ) {
			ret = true;
			break;
		}
	}
	return ret;
}

void MyThreadPool::start( int numThreads ) {
	setNumThreads( numThreads );
	start();
}

void MyThreadPool::start() {
	running_ = true;
	threads_.resize( numThreads_ );
	try {
		for ( int i = 0; i < numThreads_; ++i ) {
			queues_.emplace_back( new ds::StealingQueue<Task>() );
			threads_[ i ] = std::thread( 
					std::bind( &MyThreadPool::threadRoutine, this, i )
					);
		}
	} catch ( ... ) {
		running_ = false;
		throw;
	}
}

void MyThreadPool::run( Task task ) {
	if ( private_queue_ ) {
		private_queue_->push_back( std::move( task ) );
	} else {
		shared_queue_.push( std::move( task ) );
	}
}


}
