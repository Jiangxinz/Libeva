#ifndef _MINIMEMCACHED_UTILITY_STRINGTOKENIZER_H_
#define _MINIMEMCACHED_UTILITY_STRINGTOKENIZER_H_

#include <string_view>
#include <functional>

template<
	typename T,
	typename SepFunc >
class Tokenizer {
public:
	Tokenizer( const T& s, SepFunc sepFunc );
	// ~StringTokenizer()  {}

	bool hasMoreToken() const
	{ return beg_ != end_; }

	std::string_view nextToken();
private:
	typedef typename T::iterator Iterator;
	Iterator beg_;
	Iterator end_;
	SepFunc sepFunc_;
};


typedef std::function<bool(char)> SepFunc;

typedef Tokenizer<std::string_view, SepFunc> StringTokenizer;

#include "minimemcached/utility/Tokenizer.cc"


#endif /* _MINIMEMCACHED_UTILITY_STRINGTOKENIZER_H_ */
