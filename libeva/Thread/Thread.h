#ifndef _LIBEVA_THREAD_THREAD_H_
#define _LIBEVA_THREAD_THREAD_H_

#include "libeva/Thread/CountDownLatch.h"
#include <atomic>
#include <functional>
#include <memory>
#include <pthread.h>
#include <assert.h>
#include <string>

using std::string;

namespace libeva
{

class Thread : noncopyable
{
 public:
  typedef std::function<void ()> ThreadFunc;

  explicit Thread(ThreadFunc, const string& name = string());
  // FIXME: make it movable in C++11
  ~Thread();

  void start();
  int join(); // return pthread_join()

  bool started() const { return started_; }
  // pthread_t pthreadId() const { return pthreadId_; }
  pid_t tid() const { return tid_; }
  const string& name() const { return name_; }

  // TODO:
  static int numCreated() { return numCreated_.load(std::memory_order_acquire);; }

 private:
  void setDefaultName();

  bool       started_;
  bool       joined_;
  pthread_t  pthreadId_;
  pid_t      tid_;
  ThreadFunc func_;
  string     name_;
  CountDownLatch latch_;

  static std::atomic_int numCreated_;
};

}  // 

#endif  
