#include "libeva/Reactor/ReactorThread.h"
#include "libeva/Reactor/Reactor.h"

namespace libeva {

ReactorThread::ReactorThread( const ThreadInitCallback &cb ) : reactor_ ( nullptr ), 
                    thread_( std::bind( &ReactorThread::threadFunc, this ) ),
       		    callback_( cb ) {
}

ReactorThread::~ReactorThread() {
    Reactor *preactor = reactor_.load(std::memory_order_acquire );

//     printf( "Currenttid=%d, ReactorThread::~ReactorThread()\n",
// 		    CurrentThread::tid() );
    if ( preactor != nullptr ) {
	    // printf("preactor != nullptr\n");
        preactor->stop();
        thread_.join();
    }
}

void ReactorThread::threadFunc() {
    
    Reactor reactor;
   
    if ( callback_ ) {
	    callback_( &reactor );
    }

    reactor_.store( &reactor, std::memory_order_release );
    reactor.dispatch();
    
    reactor_.store( nullptr, std::memory_order_release );
}

Reactor* ReactorThread::startDispatch() {
    thread_.start();
    while ( reactor_.load( std::memory_order_acquire ) == nullptr );
    Reactor *ret = reactor_.load( std::memory_order_acquire );
    return ret;
}

}
