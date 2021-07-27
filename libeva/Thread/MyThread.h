#ifndef _LIBEVA_THREAD_MYTHREAD_H_
#define _LIBEVA_THREAD_MYTHREAD_H_

#include "libeva/noncopyable.h"

namespace libeva {

class MyThread : libeva:noncopyable {
public:	
	typedef std::function<void(void)> ThreadFunc;
	explicit Thread( ThreadFunc );
	~Thread();


	void start();
	int join();


	bool started() const
	{ return started_; }

	pid_t tid() const
	{ return tid_; }

private:
};

}

#endif /* _LIBEVA_THREAD_MYTHREAD_H_ */
