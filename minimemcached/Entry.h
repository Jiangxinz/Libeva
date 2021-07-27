#ifndef _MINIMEMCACHED_ENTRY_H_
#define _MINIMEMCACHED_ENTRY_H_

#include <memory>
#include <iostream>

#include <string>
#include <string.h> // memmove
#include <cassert>
#include <string_view>
// #include <boost/functional/hash/hash.hpp>
#include "libeva/Net/Buffer.h"


// #include "utility/MaxVal.h"

class Entry;

typedef std::shared_ptr<Entry> EntryPtr;
typedef std::shared_ptr<const Entry>  ConstEntryPtr;

class Entry {
public:
	Entry();
	explicit Entry( const std::string_view &key,
		        int valueLen=256 );

	Entry( const char *data, int keyLen, int valueLen, int free );

	~Entry() {
		free( data_ );
		data_ = nullptr;
	}

	void append( const char* data, size_t len );

	void resetKey( std::string_view key );
	
        std::size_t hash_str( std::string_view str ) const;


	void resetKey( std::string_view key, int valueLen );

	void retrieve( int numBytes ) {
		assert ( static_cast<int>( free_ ) >= numBytes );
		free_ -= numBytes;
	}

	void Reset() {
		free_ += keyLen_ + valueLen_;
		keyLen_ = valueLen_ = 0;
	}

	char *data() const {
		return data_;
	}

	size_t numFree() const
	{ return free_; }

	int length() const
	{ return keyLen_ + valueLen_; }

	std::string_view key() const
	{ return std::string_view( data_, keyLen_ ); }

	void copyToBuffer( libeva::Buffer *buf ) {
		buf->append( "VALUE " );
		buf->append( data_, keyLen_ );
		buf->append( "\r\n" );
		buf->append( data_+keyLen_, valueLen_ );
	}

	size_t hash() const
	{ return hash_; }

	int keyLen() const 
	{ return keyLen_; }

	int valueLen() const
	{ return valueLen_; }

	static EntryPtr clone( const char *data, int keyLen, int valueLen, int free ) {
		return std::make_shared<Entry>( data, keyLen, valueLen, free );
	}

private:
	// FIXME: use size_t 
	int keyLen_;
	int valueLen_;
	int free_;
	char *data_;
	size_t hash_;
};

// template <>
// struct MaxVal<Entry> {
// 	static const Entry value;
// };
// 
// template<>
// static const Entry MaxVal<Entry>::value("zzzz");


#endif /* _MINIMEMCACHED_ENTRY_H_ */
