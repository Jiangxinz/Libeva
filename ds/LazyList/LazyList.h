#ifndef _DS_LAZYLIST_H_
#define _DS_LAZYLIST_H_

#include <iostream>
#include <climits>
#include <cassert>
#include <memory>
#include <functional>
#include <unordered_set>
#include "libeva/noncopyable.h"
#include "utility/SpinLock.h"
// #include "utility/max_traits.h"

namespace ds {

template<typename T, 
	typename Compare = std::less<T>>
class LazyList : libeva::noncopyable {

	struct Node {
		Node() :
			entry_(),
			next_( nullptr ),
	       		marked_( false ) {}

		Node( const T &entry) :
			entry_( entry ),
	       		next_( nullptr ),
	       		marked_( false ) {}

		Node( T &&entry ) noexcept :
			entry_( std::move( entry ) ),
			next_( nullptr ),
			marked_( false ) {}

		~Node() = default;

		const T& key() const
		{ return entry_; }


		SpinLock& getLock()
		{ return spinLock_; }

		bool marked() const
		{ return marked_; }

		void mark( bool flag )
		{ marked_ = flag; }

		T entry_;
		Node *next_;
		mutable SpinLock spinLock_;
		bool marked_;
	};
	typedef Node EntryNode;
public:
	LazyList() :
		head_( new EntryNode() ),
		tail_( new EntryNode() ) {
	}

	explicit LazyList( const T &max_val ):
		head_( new EntryNode( ) ),
		// TODO: static
		tail_( new EntryNode( max_val ) )
	{
		head_->next_ = tail_;
	}
	~LazyList() {
		for ( Node *p = head_; p != tail_ ; ) {
			Node *next = p->next_;
			delete p;
			p = next;
		}
		for ( EntryNode *node : garbage_ ) {
			delete node;
		}
	}

	bool remove( const T &key );

	bool add( T key );

	const T get( const T &key ) const ;

	bool contains( const T &key );

	bool validate( EntryNode *pred, EntryNode *curr ) {
		return !pred->marked() && !curr->marked() && pred->next_ == curr;
	}

	bool equal( const T& lhs, const T& rhs ) const
	{ 
		bool ret = ( !cmp_( lhs, rhs ) && !cmp_( rhs, lhs ) ); 
		// if ( ret ) {
		// 	std::cout << "[" << lhs->key() << "] equal to [" << rhs->key() << "]\n";
		// } else {
		// 	std::cout << "[" << lhs->key() << "] not equal to [" << rhs->key() << "]\n";
		// }
		return ret;
	}

	void traversal( const std::function<void(const T&)> func ) {
		EntryNode* curr = head_->next_;
		while ( curr != tail_ ) {
			func( curr->key() );
			curr = curr->next_;
		}
	}

	void setMaxKey( T key )
	{ tail_->entry_ = std::move( key ); }

private:
	EntryNode* head_;
	EntryNode* tail_;
	Compare cmp_;
	std::unordered_set<EntryNode*> garbage_;
};	

// template<typename T, 
// 	typename Compare>
// __attribute__((weak)) typename LazyList<T, Compare>::EntryNode LazyList<T, Compare>::sentry;


template<typename T, typename Compare >
bool LazyList<T, Compare>::remove( const T &key ) {
	while( true ) {
		EntryNode *pred = head_;
		EntryNode *curr = head_->next_;
		while ( cmp_( curr->key(), key ) ) {
			pred = curr;
			curr = curr->next_;
		}
		SpinLockGuard outterGuard( pred->getLock() );
		SpinLockGuard innerGuard( curr->getLock() );
		if ( validate( pred, curr ) ) {
			if ( !equal( curr->key(), key ) ) {
				return false;
			} else {
				curr->mark( true );
				pred->next_ = curr->next_;
				// 不应该删除curr
				// 应该加到某个地方存起来
				// FIXME: hazard pointer
				assert( garbage_.find( curr ) == garbage_.end() );
				// FIXME: move out side of spin lock
				// garbage_.insert( curr );
				delete curr;
				return true;
			}
		}
	}
	return false;
}

template<typename T, typename Compare >
bool LazyList<T, Compare>::contains( const T &key ) {
	EntryNode *curr = head_;
	while ( cmp_( curr->key(), key ) ) {
		curr = curr->next_;
	}
	return equal( curr->key(), key ) && !curr->marked();
}

template<typename T, typename Compare >
// TODO: const T&key
bool LazyList<T, Compare>::add( T key ) {
	while ( true ) {
		EntryNode *pred = head_;
		EntryNode *curr = head_->next_;
		while ( cmp_( ( curr->key() ), key ) ) {
			pred = curr;
			curr = curr->next_;
		}
		SpinLockGuard outterGuard( pred->getLock() );
		SpinLockGuard innerGuard( curr->getLock() );
		if ( validate( pred, curr ) ) {
			if ( equal( curr->key(), key ) ) {
				curr->entry_ = std::move( key );
				return true;
			} else {
				EntryNode *entry = new EntryNode( std::move( key ) );
				entry->next_ = curr;
				pred->next_ = entry;
				return true;
			}
		}
	}
	assert( false );
	return false;
}

template<typename T, typename Compare >
const T LazyList<T, Compare>::get( const T &key ) const {
	// printf("traversial:[\n");
	// for ( EntryNode *p = head_->next_; p != tail_; p = p->next_ ) {
	// 	std::cout << p->key()->key() << ",";
	// }
	// printf("]\n");
	EntryNode *curr = head_->next_;
	while ( cmp_( curr->key(), key ) ) {
		curr = curr->next_;
	}
	// bool equal( curr->key(), key )
	return ( curr->marked() || !equal( curr->key(), key ) ) ? T() : curr->key();
}

}

#endif /* _DS_LAZYLIST_H_ */
