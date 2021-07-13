#ifndef _DS_HASHSET_H_
#define _DS_HASHSET_H_

#include <memory>
#include <iostream>
#include <array>
#include "ds/LazyList/LazyList.h"

namespace ds {


// Fixed number of buckets

template<typename Key,
	unsigned Len = 1024,
	typename Compare = std::less<Key>,
	typename Hash = std::hash<Key>>
class HashSet {
public:
	HashSet() = default;
	explicit HashSet( const Key &max_val );
	~HashSet() = default;
	bool remove( const Key& key );
	bool add( Key key );
	bool contains( const Key& key );
	const Key get( const Key& key ) const;
	void setMaxKey( const Key& key );
private:
	typedef LazyList<Key, Compare> LinkedList;
	// std::vector<LinkedList> buckets;
	std::array<std::unique_ptr<LinkedList>, Len> buckets_;
};

template<typename Key,
	unsigned Len,
	typename Compare,
	typename Hash>
HashSet<Key, Len, Compare, Hash>::HashSet( const Key &max_key ) {
	for ( unsigned i = 0; i < Len; ++i ) {
		buckets_[ i ] = std::make_unique<LinkedList>( max_key );
	}
}

template<typename Key,
	unsigned Len,
	typename Compare,
	typename Hash>
void HashSet<Key, Len, Compare, Hash>::setMaxKey( const Key &max_key ) {
	for ( unsigned i = 0; i < Len; ++i ) {
		buckets_[ i ]->setMaxKey( max_key );
	}
}

template<typename Key,
	unsigned Len,
	typename Compare,
	typename Hash>
bool HashSet<Key, Len, Compare, Hash>::remove( const Key& key ) {
	unsigned long idx = Hash()( key ) % Len;
	// unsigned long idx = key->hash();
	LinkedList *mylist = buckets_.at( idx ).get();
	return mylist->remove( key );
}

template<typename Key,
	unsigned Len,
	typename Compare,
	typename Hash>
bool HashSet<Key, Len, Compare, Hash>::add( Key key ) {
	unsigned long idx = Hash()( key ) % Len;
	LinkedList *mylist = buckets_.at( idx ).get();
	return mylist->add( std::move( key ) );
}

template<typename Key,
	unsigned Len,
	typename Compare,
	typename Hash>
bool HashSet<Key, Len, Compare, Hash>::contains( const Key& key ) {
	// TODO: use get
	unsigned idx = Hash()( key ) % Len;
	LinkedList *mylist = buckets_.at( idx ).get();
	return mylist->contains( key );
}

template<typename Key,
	unsigned Len,
	typename Compare,
	typename Hash>
const Key HashSet<Key, Len, Compare, Hash>::get( const Key& key ) const {
	unsigned long idx = Hash()( key ) % Len;
	const LinkedList *mylist = buckets_.at( idx ).get();
	return mylist->get( key );
}

}


#endif /* _DS_HASHSET_H_ */
