// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "libeva/Thread/CountDownLatch.h"

using namespace libeva;

CountDownLatch::CountDownLatch(int count)
  : mutex_(),
    condition_(),
    count_( count )
{
}

void CountDownLatch::wait()
{
  std::unique_lock<std::mutex> guard( mutex_ );
  condition_.wait(guard, [&] { return count_ == 0; } );
}

void CountDownLatch::countDown()
{
  std::lock_guard<std::mutex> guard( mutex_ );
  --count_;
  if ( count_ == 0 )
  {
    condition_.notify_all();
  }
}

int CountDownLatch::getCount() const
{
  std::lock_guard<std::mutex> guard( mutex_ );
  return count_;
}

