#include "libeva/Reactor/ReactorThreadPool.h"
#include "libeva/Reactor/Reactor.h"
#include "libeva/Thread/Thread.h"

#include <stdio.h>
#include <unistd.h>

using namespace libeva;

void print(Reactor* p = NULL)
{
  printf("main(): pid = %d, tid = %d, loop = %p\n",
         getpid(), CurrentThread::tid(), p);
}

void init(Reactor* p)
{
  printf("init(): pid = %d, tid = %d, loop = %p\n",
         getpid(), CurrentThread::tid(), p);
}

int main()
{
  print();

  Reactor reactor;
  reactor.runAfter(11, std::bind(&Reactor::stop, &reactor));

  {
    printf("Single thread %p:\n", &reactor);
    ReactorThreadPool model( &reactor );
    model.setThreadNum(0);
    model.start(init);
    assert(model.getNextReactor() == &loop);
    assert(model.getNextReactor() == &loop);
    assert(model.getNextReactor() == &loop);
  }

  {
    printf("Another thread:\n");
    ReactorThreadPool model( &reactor );
    model.setThreadNum(0);
    model.start(init);
    Reactor* nextReactor = model.getNextReactor();
    nextReactor->runAfter(2, std::bind(print, nextReactor));
    assert(nextReactor != &reactor);
    assert(nextReactor == model.getNextReactor());
    assert(nextReactor == model.getNextReactor());
    ::sleep(3);
  }

  {
    printf("Three threads:\n");
    ReactorThreadPool model(&reactor);
    model.setThreadNum(3);
    model.start(init);
    Reactor* nextReactor = model.getNextReactor();
    nextReactor->runAfter(2, std::bind(print, nextReactor));
    assert(nextReactor != &reactor);
    assert(nextReactor == model.getNextReactor());
    assert(nextReactor == model.getNextReactor());
  }

  reactor.dispatch();
}

