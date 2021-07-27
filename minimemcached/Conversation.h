#ifndef _MINIMEMCACHED_CONVERSATION_H_
#define _MINIMEMCACHED_CONVERSATION_H_

#include <vector>
#include <string>
#include <string_view>

#include "libeva/Callbacks.h"
#include "minimemcached/MiniMemcached.h"
#include "minimemcached/utility/Tokenizer.h"
#include "minimemcached/Entry.h"

class MiniMemcached;

class Conversation {
	typedef std::function<void(StringTokenizer&, void*)> HandleFunc;
	enum STATE {
		kNewCommand,
		kReceiveValue,
		kDiscardValue
	};
public:
	Conversation( MiniMemcached *server, const libeva::TcpConnectionPtr &conn );
	~Conversation();
private:
	void TestHandler( StringTokenizer& token, void *arg );
	void GetHandler( StringTokenizer& token, void *arg );
	void SetHandler( StringTokenizer& token, void *arg );

	void receiveValue( libeva::Buffer* buf );
private:
	void reply( std::string_view msg );
	void msgCb( const libeva::TcpConnectionPtr &conn, libeva::Buffer*, libeva::Timestamp );
	bool handle_command( const std::string_view req );
	std::vector<std::string_view> splitBySpace( const std::string_view str );

	void resetRequest();
	inline bool isBinaryProtocol( uint8_t firstByte )
	{ return firstByte == 0x80 ? true : false; }

	MiniMemcached *server_;
	libeva::TcpConnectionPtr conn_;
	std::unordered_map<std::string, HandleFunc> controller_;
	EntryPtr currEntry_;
	std::string command_;
	STATE state_;
	libeva::Buffer outputBuf_;
};

#endif /* _MINIMEMCACHED_CONVERSATION_H_ */
