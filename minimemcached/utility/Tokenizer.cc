#include <iostream>
#include "minimemcached/utility/Tokenizer.h"


template<
	typename T,
	typename SepFunc >
Tokenizer<T, SepFunc>::Tokenizer( const T&s, SepFunc sepFunc ) :
	beg_( s.begin() ), end_( s.end() ), sepFunc_( sepFunc )
{ }

template<
	typename T,
	typename SepFunc >
std::string_view Tokenizer<T, SepFunc>::nextToken() {
	while ( beg_ != end_ && sepFunc_( *beg_ ) ) {
		++beg_;
	}
	Iterator it = beg_;
	for( ; it != end_ && !sepFunc_( *it ); ++it );
	Iterator old_beg = beg_;
	beg_ = it;
	std::string_view ret( old_beg, it-old_beg );
	// std::cout << "Token:(" << ret << ")" << std::endl;
	return ret;
}

