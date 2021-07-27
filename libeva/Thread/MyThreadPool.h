#ifndef _LIBEVA_THREAD_THREADPOOL_H_
#define _LIBEVA_THREAD_THREADPOOL_H_

#include <atomic>
#include <vector>
#include <thread>
#include <functional>
#include <memory>
#include "libeva/noncopyable.h"
#include "libeva/Thread/ThreadsJoiner.h"
#include "libeva/Thread/CountDownLatch.h"
#include "ds/FineGrainedQueue/FineGrainedQueue.h"
#include "ds/StealingQueue/StealingQueue.h"

namespace libeva {


class MyThreadPool : libeva::noncopyable {
	typedef std::function<void(void)> Task;
public:
	MyThreadPool();
	~MyThreadPool() {
		running_ = false;
	}

	void setNumThreads( int numThreads )
	{ numThreads_ = numThreads; }

	void start( int numThreads );
	void start();
	
	void stop() {
		running_ = false;
	}

	void run( Task task );

private:
	void threadRoutine( unsigned idx );
	bool stealWork( Task &task );

	int numThreads_;
	std::atomic_bool running_;
	ds::FineGrainedQueue<Task> shared_queue_;

	typedef std::unique_ptr<ds::StealingQueue<Task>> StealingQueuePtr;

	std::vector<StealingQueuePtr> queues_;

	std::vector<std::thread> threads_;
	ThreadsJoiner<std::vector<std::thread>> joiner_; // joiner_ should destruct before threads_

	static thread_local ds::StealingQueue<Task>* private_queue_;
	static thread_local int idx_;
};

}


#endif /* _LIBEVA_THREAD_THREADPOOL_H_ */
