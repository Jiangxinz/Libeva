#include "minimemcached/Entry.h"

Entry::Entry() : keyLen_( 1 ), valueLen_( 0 ), free_( 0 ),
	data_( static_cast<char*>( ::malloc( 1 ) ) ) {
			data_[ 0 ] = '\0';
	}


Entry::Entry( const std::string_view &key,
	        int valueLen ) :
	keyLen_( 0 ),
	valueLen_( 0 ),
	free_( static_cast<int>(key.size())+valueLen ),
	data_( static_cast<char*>( ::malloc( static_cast<int>(key.size())+valueLen ) ) ) {
	assert( free_ - static_cast<int>( key.size() ) >= 2 );
	resetKey( key );
}

Entry::Entry( const char *data, int keyLen, int valueLen, int free ) :
	keyLen_( keyLen ), valueLen_( valueLen ), free_( free ),
	data_( static_cast<char*>( ::malloc( keyLen+valueLen+free ) ) ) {
	memmove( data_, data, keyLen+valueLen );
	std::string_view key( data, keyLen );
	hash_ = hash_str( key );
}

void Entry::append( const char* data, size_t len ) {
	assert ( static_cast<int>( len ) <= free_ );
	memmove( data_+length(), data, len );
	valueLen_ += static_cast<int>( len );
	retrieve( static_cast<int>( len ) );
}

void Entry::resetKey( std::string_view key ) {
	free_ += keyLen_ + valueLen_;
	keyLen_ = static_cast<int>( key.size() );
	valueLen_ = 0;
	memmove( data_, key.data(), keyLen_ );
	retrieve( keyLen_ );
	// hash_ = boost::hash_range( key.begin(), key.end() );
	hash_ = hash_str( key );
}

std::size_t Entry::hash_str( std::string_view str ) const {
        // return entryPtr->hash();
        std::size_t ret = 0;
        const std::size_t p = 131;
        for ( size_t i = 0; i < str.size(); ++i ) {
                ret = ret*p + str[ i ];
        }
        return ret;
}


void Entry::resetKey( std::string_view key, int valueLen ) {
	int curTotal = keyLen_ + valueLen_ + free_;
	// printf("curTotal=%d\n", curTotal);
	int reqTotal = static_cast<int>( key.size() ) + valueLen;
	// printf("reqTotal=%d\n", reqTotal);
	keyLen_ = static_cast<int>( key.size() );
	if ( curTotal < reqTotal ) {
		free( data_ );
		data_ = static_cast<char*>( ::malloc( keyLen_ + valueLen ) );
	}
	memmove( data_, key.data(), keyLen_ );
	valueLen_ = 0;
	free_ = valueLen;
	hash_ = hash_str( key );
}

