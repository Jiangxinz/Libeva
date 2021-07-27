#include "libeva/Reactor/ReactorThread.h"
#include "libeva/Reactor/Reactor.h"
#include "libeva/Thread/Thread.h"
#include "libeva/Thread/CountDownLatch.h"

#include <stdio.h>
#include <chrono>
#include <unistd.h>
#include <thread>

using namespace libeva;

void print(Reactor* p = NULL)
{
  printf("print: pid = %d, tid = %d, loop = %p\n",
         getpid(), CurrentThread::tid(), p);
}

void quit(Reactor* p)
{
  print(p);
  p->stop();
}

int main()
{
  print();

  {
  ReactorThread thr1;  // never start
  }

  {
  // dtor calls quit()
  ReactorThread thr2;
  Reactor* reactor = thr2.startDispatch();
  reactor->runInReactor(std::bind(print, reactor));
  std::this_thread::sleep_for(std::chrono::milliseconds(5 * 1000));
  }

  {
  // quit() before dtor
  ReactorThread thr3;
  Reactor* reactor = thr3.startDispatch();
  reactor->runInReactor(std::bind(quit, reactor));
  std::this_thread::sleep_for(std::chrono::milliseconds(5 * 1000));
  }
}

