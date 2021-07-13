#include <mutex>
#include "libeva/Thread/CurrentThread.h"
#include "libeva/Thread/Thread.h"
#include "libeva/Timestamp.h"

#include <map>
#include <string>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

std::mutex g_mutex;
std::map<int, int> g_delays;

void threadFunc()
{
  //printf("tid=%d\n", libeva::CurrentThread::tid());
}

void threadFunc2(libeva::Timestamp start)
{
  libeva::Timestamp now(libeva::Timestamp::now());
  int delay = static_cast<int>(timeDifference(now, start) * 1000000);
  std::lock_guard<std::mutex> lock(g_mutex);
  ++g_delays[delay];
}

void forkBench()
{
  sleep(10);
  libeva::Timestamp start(libeva::Timestamp::now());
  int kProcesses = 10*1000;

  for (int i = 0; i < kProcesses; ++i)
  {
    pid_t child = fork();
    if (child == 0)
    {
      exit(0);
    }
    else
    {
      waitpid(child, NULL, 0);
    }
  }

  double timeUsed = timeDifference(libeva::Timestamp::now(), start);
  printf("process creation time used %f us\n", timeUsed*1000000/kProcesses);
  printf("number of created processes %d\n", kProcesses);
}

int main(int argc, char* argv[])
{
  printf("pid=%d, tid=%d\n", ::getpid(), libeva::CurrentThread::tid());
  libeva::Timestamp start(libeva::Timestamp::now());

  int kThreads = 100*1000;
  for (int i = 0; i < kThreads; ++i)
  {
    libeva::Thread t1(threadFunc);
    t1.start();
    t1.join();
  }

  double timeUsed = timeDifference(libeva::Timestamp::now(), start);
  printf("thread creation time %f us\n", timeUsed*1000000/kThreads);
  printf("number of created threads %d\n", libeva::Thread::numCreated());

  for (int i = 0; i < kThreads; ++i)
  {
    libeva::Timestamp now(libeva::Timestamp::now());
    libeva::Thread t2(std::bind(threadFunc2, now));
    t2.start();
    t2.join();
  }
  {
    std::lock_guard<std::mutex> lock(g_mutex);
    for (const auto& delay : g_delays)
    {
      printf("delay = %d, count = %d\n",
             delay.first, delay.second);
    }
  }

  forkBench();
}
