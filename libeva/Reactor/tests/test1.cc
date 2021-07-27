#include <pthread.h>
#include <stdio.h>
#include "libeva/Thread/Thread.h"
#include "libeva/Reactor/Reactor.h"

void threadFunc() {
    printf("threadFunc: pid = %d, tid = %d\n",
            getpid(), libeva::CurrentThread::tid());
    libeva::Reactor reactor;
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