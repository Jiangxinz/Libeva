#ifndef _DS_STEALING_QUEUE_H_
#define _DS_STEALING_QUEUE_H_

#include <mutex>
#include <deque>
#include "libeva/noncopyable.h"

namespace ds {

template<typename T>
class StealingQueue : libeva::noncopyable {
public:
	StealingQueue() = default;
	void push_back( T val );
	bool tryPopBack( T &ret );
	bool tryStealFront( T &ret );
	bool empty() const;
private:
	std::deque<T> queue_;
	mutable std::mutex mutex_;
};


template<typename T>
void StealingQueue<T>::push_back( T val ) {
	std::lock_guard<std::mutex> lock( mutex_ );
	queue_.emplace_back( std::move( val ) );
}

template<typename T>
bool StealingQueue<T>::tryPopBack( T &ret ) {
	std::lock_guard<std::mutex> lock( mutex_ );
	if ( queue_.empty() ) {
		return false;
	}
	ret = std::move( queue_.back() );
	queue_.pop_back();
	return true;
}

template<typename T>
bool StealingQueue<T>::tryStealFront( T &ret ) {
	std::lock_guard<std::mutex> lock( mutex_ );
	if ( queue_.empty() ) {
		return false;
	}
	ret = std::move( queue_.front() );
	queue_.pop_front();
	return true;
}

template<typename T>
bool StealingQueue<T>::empty() const {
	std::lock_guard<std::mutex> lock( mutex_ );
	return queue_.empty();
}

}



#endif /* _DS_STEALING_QUEUE_H_ */
