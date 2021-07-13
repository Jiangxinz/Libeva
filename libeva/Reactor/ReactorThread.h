#ifndef _LIBEVA_REACTORTHREAD_H_
#define _LIBEVA_REACTORTHREAD_H_

#include <mutex>
#include <condition_variable>
#include "libeva/Thread/Thread.h"
#include "libeva/noncopyable.h"

namespace libeva {

class Reactor;

class ReactorThread : public noncopyable {
public:
	typedef std::function< void(Reactor*) > ThreadInitCallback;
	ReactorThread( const ThreadInitCallback &cb = ThreadInitCallback() );
	~ReactorThread();
	
	Reactor* startDispatch();
	
	void threadFunc();

private:
	std::atomic<Reactor*> reactor_;
	Thread thread_;
	ThreadInitCallback callback_;
};

}

#endif /* _LIBEVA_REACTORTHREAD_H_ */
