#include <pthread.h>
#include <stdio.h>
#include "libeva/Thread/Thread.h"
#include "libeva/Reactor/Reactor.h"


libeva::Reactor *g_reactor;

void threadFunc() {
    g_reactor->dispatch();
}

int main() 
{
    libeva::Reactor reactor;
    g_reactor = &reactor;
    libeva::Thread thread(threadFunc);
    thread.start();
    thread.join();
}