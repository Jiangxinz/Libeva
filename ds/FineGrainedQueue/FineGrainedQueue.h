#ifndef _DS_FINEGRAINEDQUEUE_H_
#define _DS_FINEGRAINEDQUEUE_H_

#include <mutex>
#include <memory>
#include <condition_variable>
#include "libeva/noncopyable.h"

// FIXME: move implementation to cpp file ?

namespace ds {

template <typename T>
class FineGrainedQueue : libeva::noncopyable {
	// typedef T WrapperT;
	struct Node {
		T entry_;
		Node* next_;
	};
public:
	FineGrainedQueue() :
		head_( new Node() ), tail_( head_ ), notEmpty_() {

	}
	~FineGrainedQueue();

	// T tryPop();
        bool tryPop( T& val );
	T waitAndPop();
	// void waitAndPop( T& val );
	void push( T val );
	void empty();

private:
	Node* getTail() {
		std::lock_guard<std::mutex> tail_guard( tail_mutex_ );
		return tail_;
	}

	Node *head_;
	std::mutex head_mutex_;

	Node *tail_;
	std::mutex tail_mutex_;

	std::condition_variable notEmpty_;
};

// template<typename T>
// FineGrainedQueue<T>::FineGrainedQueue() :
// 	head_( new Node ), tail_( head_ ), cond_() {}

template<typename T>
FineGrainedQueue<T>::~FineGrainedQueue() {
	// delete head_;
	// TODO: delete all
	while ( head_ ) {
		Node *tmp = head_;
		head_ = head_->next_;
		delete tmp;
	}
}

template<typename T>
void FineGrainedQueue<T>::push( T val ) {
	std::unique_ptr<Node> newNode = std::make_unique<Node>();
	{
	std::lock_guard<std::mutex> guard( tail_mutex_ );
	tail_->entry_ = std::move( val );
	tail_->next_ = newNode.release();
	tail_ = tail_->next_;
	}
	notEmpty_.notify_all();
}

template<typename T>
T FineGrainedQueue<T>::waitAndPop() {
	Node *retNode = nullptr;
	{
	std::unique_lock<std::mutex> head_guard( head_mutex_ );
	notEmpty_.wait( head_guard, [&] {
			return head_ != getTail();
	});
	retNode = head_;
	head_ = head_->next_;
	}
	T ret( std::move( retNode->entry_ ) );
	delete retNode;
	return ret;
}

template<typename T>
bool FineGrainedQueue<T>::tryPop( T& ret ) {
	Node *tobeDeleted = nullptr;
	{
	std::unique_lock<std::mutex> head_guard( head_mutex_ );
	if ( head_ == getTail() ) {
			return false;
	}
	ret = std::move( head_->entry_ );
	tobeDeleted = head_;
	head_ = head_->next_;
	}
	delete tobeDeleted;
	return true;
}

}



#endif /* _DS_FINEGRAINEDQUEUE_H_ */
