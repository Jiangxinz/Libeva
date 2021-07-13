#include "libeva/Reactor/ReactorThreadPool.h"
#include "libeva/Reactor/ReactorThread.h"
#include "libeva/Reactor/Reactor.h"

namespace libeva {

void ReactorThreadPool::start( const ThreadInitCallback &cb ) {
	reactor_->assertInDispatchingThread();
	for ( int i = 0; i < threadNum_; ++i ) {
		ReactorThread *rtThread = new ReactorThread( cb );
		reactorThreads_.emplace_back( rtThread );
		reactors_.push_back( rtThread->startDispatch() );
	}
	if ( threadNum_ == 0 && cb ) {
		cb( reactor_ );
	}
}

Reactor* ReactorThreadPool::getNextReactor() {
	Reactor *ret = reactor_;
	if ( threadNum_ > 0 ) {
		ret = reactors_.at( nextIndex_ );
		nextIndex_ = ( nextIndex_+1 ) % threadNum_;
	}
	return ret;
}

}
