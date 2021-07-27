#ifndef _LIBEVA_REACTORTHREADPOOL_H_
#define _LIBEVA_REACTORTHREADPOOL_H_

#include <memory>
#include <vector>

#include "libeva/Reactor/ReactorThread.h"

namespace libeva {

class Reactor;

class ReactorThreadPool {
public:
	typedef std::function< void(Reactor*) > ThreadInitCallback;
	ReactorThreadPool( Reactor *reactor ) :
		reactor_( reactor ),
		reactorThreads_(),
		reactors_(),
		threadNum_( 0 ),
       		nextIndex_( 0 ) {
	}
	~ReactorThreadPool() {
	}

	void start( const ThreadInitCallback &cb );
	void start( );

	Reactor* getNextReactor() ;

	void setThreadNum( int threadNum )
	{ threadNum_ = threadNum; }
private:
	Reactor *reactor_;
	std::vector<std::unique_ptr<ReactorThread>> reactorThreads_;
	std::vector<Reactor*> reactors_;
	int threadNum_;
	int nextIndex_;
};

}
#endif /* _LIBEVA_REACTORTHREADPOOL_H_ */
