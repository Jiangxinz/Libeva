#ifndef _MINIMEMCACHED_ENTRY_H_
#define _MINIMEMCACHED_ENTRY_H_

#include <memory>

#include <string>
#include <string.h> // memmove
#include <cassert>
#include <string_view>
#include <boost/functional/hash/hash.hpp>
#include "libeva/Net/Buffer.h"


// #include "utility/MaxVal.h"

// TODO: move to cpp file

class Entry {
public:
	Entry();
	explicit Entry( const std::string_view &key,
		        int valueLen=256 ) :
		keyLen_( 0 ),
       		valueLen_( 0 ),
		free_( static_cast<int>(key.size())+valueLen ),
		data_( static_cast<char*>( ::malloc( static_cast<int>(key.size())+valueLen ) ) ) {
		assert( free_ - static_cast<int>( key.size() ) >= 2 );
		resetKey( key );
	}

	~Entry() {
		free( data_ );
		data_ = nullptr;
	}

	void append( const char* data, size_t len ) {
		assert ( static_cast<int>( len ) <= free_ );
		memmove( data_+length(), data, len );
		valueLen_ += static_cast<int>( len );
		retrieve( static_cast<int>( len ) );
	}

	void resetKey( std::string_view key ) {
		free_ += keyLen_ + valueLen_;
		keyLen_ = static_cast<int>( key.size() );
		valueLen_ = 0;
		memmove( data_, key.data(), keyLen_ );
		retrieve( keyLen_ );
		hash_ = boost::hash_range( key.begin(), key.end() );
	}

	void retrieve( int numBytes ) {
		assert ( static_cast<int>( free_ ) >= numBytes );
		free_ -= numBytes;
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

typedef std::shared_ptr<Entry> EntryPtr;
typedef std::shared_ptr<const Entry>  ConstEntryPtr;

#endif /* _MINIMEMCACHED_ENTRY_H_ */
