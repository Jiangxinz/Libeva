
#ifndef _LIBEVA_COUNTDOWNLATCH_H_
#define _LIBEVA_COUNTDOWNLATCH_H_

#include <mutex>
#include <condition_variable>
#include "libeva/noncopyable.h"

namespace libeva
{

class CountDownLatch : noncopyable
{
 public:

  explicit CountDownLatch(int count);

  void wait();

  void countDown();

  int getCount() const;

 private:
  mutable std::mutex mutex_;
  std::condition_variable condition_;
  int count_;

};  // namespace libeva

}
#endif  // _LIBEVA_COUNTDOWNLATCH_H_
