#ifndef _UTILITY_SPINLOCK_H_
#define _UTILITY_SPINLOCK_H_

#include <atomic>
#include "libeva/noncopyable.h"

class SpinLock : libeva::noncopyable {
public:
	SpinLock():
		flag_(ATOMIC_FLAG_INIT)
	{}
	void lock() {
		while( flag_.test_and_set( std::memory_order_acquire ) );
	}

	void unlock() {
		flag_.clear( std::memory_order_release );
	}
private:
	std::atomic_flag flag_;
};

class SpinLockGuard : libeva::noncopyable {
public:
	explicit SpinLockGuard( SpinLock& lock ) :
		locker_( lock ) 
	{ 
		locker_.lock();
	}

	~SpinLockGuard()
	{ 
		locker_.unlock();
	}
private:
	SpinLock &locker_;
};



#endif /* _UTILITY_SPINLOCK_H_ */
