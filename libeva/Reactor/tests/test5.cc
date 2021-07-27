#include <pthread.h>
#include <stdio.h>
#include "libeva/Thread/Thread.h"
#include "libeva/Reactor/Reactor.h"

libeva::Reactor *g_reactor = nullptr;
libeva::Timer *g_timer = nullptr;
int cnt = 10;

void func( int val ) {
    printf( "void func %d\n", val );
    if ( --cnt == 0 ) {
        g_reactor->stop();
    }
}

void func2( int val ) {
    printf( "void func %d\n", val );
    g_reactor->cancel( g_timer );
}

void threadFunc() {
    printf( "threadFunc: pid = %d, tid = %d\n",
            getpid(), libeva::CurrentThread::tid() );
    libeva::Reactor reactor;
    g_reactor = &reactor;
    reactor.runAfter( 1.0, std::bind( func, 1 ) );
    reactor.runAfter( 1.24, std::bind( func, 2 ) );
    reactor.runAfter( 1.0, std::bind( func, 3 ) );
    g_timer = reactor.runEvery( 1.0, std::bind( func, 4 ) );
    reactor.runAfter( 5.0, std::bind( func2, 5 ) );
    reactor.dispatch();
}

int main() 
{
    printf("main(): pid = %d, tid = %d\n",
            getpid(), libeva::CurrentThread::tid());
    // libeva::Reactor reactor;
    libeva::Thread thread(threadFunc);
    thread.start();
    thread.join();
    // reactor.dispatch();

    pthread_exit(NULL);
}