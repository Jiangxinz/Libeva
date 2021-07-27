#include "libeva/Thread/Thread.h"
#include "libeva/Thread/CurrentThread.h"

#include <string>
#include <stdio.h>
#include <unistd.h>

void mysleep(int seconds)
{
  timespec t = { seconds, 0 };
  nanosleep(&t, NULL);
}

void threadFunc()
{
  printf("tid=%d\n", libeva::CurrentThread::tid());
}

void threadFunc2(int x)
{
  printf("tid=%d, x=%d\n", libeva::CurrentThread::tid(), x);
}

void threadFunc3()
{
  printf("tid=%d\n", libeva::CurrentThread::tid());
  mysleep(1);
}

class Foo
{
 public:
  explicit Foo(double x)
    : x_(x)
  {
  }

  void memberFunc()
  {
    printf("tid=%d, Foo::x_=%f\n", libeva::CurrentThread::tid(), x_);
  }

  void memberFunc2(const std::string& text)
  {
    printf("tid=%d, Foo::x_=%f, text=%s\n", libeva::CurrentThread::tid(), x_, text.c_str());
  }

 private:
  double x_;
};

int main()
{
  printf("pid=%d, tid=%d\n", ::getpid(), libeva::CurrentThread::tid());

  libeva::Thread t1(threadFunc);
  t1.start();
  printf("t1.tid=%d\n", t1.tid());
  t1.join();

  libeva::Thread t2(std::bind(threadFunc2, 42),
                   "thread for free function with argument");
  t2.start();
  printf("t2.tid=%d\n", t2.tid());
  t2.join();

  Foo foo(87.53);
  libeva::Thread t3(std::bind(&Foo::memberFunc, &foo),
                   "thread for member function without argument");
  t3.start();
  t3.join();

  libeva::Thread t4(std::bind(&Foo::memberFunc2, std::ref(foo), std::string("Shuo Chen")));
  t4.start();
  t4.join();

  {
    libeva::Thread t5(threadFunc3);
    t5.start();
    // t5 may destruct eariler than thread creation.
  }
  mysleep(2);
  {
    libeva::Thread t6(threadFunc3);
    t6.start();
    printf("t6.tid=%d\n", t6.tid());
    mysleep(2);
    // t6 destruct later than thread creation.
  }
  printf("sleep 2 seconds\n");
  sleep(2);
  printf("number of created threads %d\n", libeva::Thread::numCreated());
}
