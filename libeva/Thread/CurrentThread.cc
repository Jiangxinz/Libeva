#include "libeva/Thread/CurrentThread.h"

#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <type_traits>

namespace libeva
{

namespace CurrentThread
{

__thread pid_t t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "unknown";
static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");

namespace {

class ThreadNameInitializer
{
 public:
  ThreadNameInitializer()
  {
    t_threadName = "main";
    CurrentThread::tid();
    // TODO: 
    // pthread_atfork(NULL, NULL, &afterFork);
  }
};

ThreadNameInitializer init;

pid_t gettid() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}


} // namespace 


void cacheTid() {
  if (t_cachedTid == 0)
  {
    t_cachedTid = gettid();
    t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}


}  // namespace CurrentThread
}  // namespace libeva
