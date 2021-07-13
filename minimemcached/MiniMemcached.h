#ifndef _MINIMEMCACHED_H_
#define _MINIMEMCACHED_H_

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <array>

#include "libeva/Reactor/Reactor.h"
#include "libeva/Net/TcpServer.h"
#include "libeva/Net/InetAddr.h"
#include "libeva/Net/TcpConnection.h"
#include "libeva/Callbacks.h"
#include "libeva/noncopyable.h"
#include "minimemcached/Entry.h"
#include "ds/HashSet/HashSet.h"

class Conversation;
typedef std::unique_ptr<Conversation> ConversationPtr;

namespace detail {

struct EntryPtrHasher {
	std::size_t operator()( const ConstEntryPtr &entryPtr ) const {
		return entryPtr->hash();
		// std::string_view str = entryPtr->key();
		// std::size_t ret = 0;
		// const std::size_t p = 131;
		// for ( size_t i = 0; i < str.size(); ++i ) {
		// 	ret = ret*p + str[ i ];
		// }
		// return ret & 0x7fffffff;
	}
};

struct EntryPtrCmp {
	bool operator()( const ConstEntryPtr &lhs, const ConstEntryPtr &rhs ) const {
		return lhs->key() < rhs->key();
	}
};

}



class MiniMemcached : libeva::noncopyable {
public:
	MiniMemcached( libeva::Reactor *reactor, libeva::InetAddr &addr );
	~MiniMemcached();

	void start()
	{ server.start(); }

	void set( const EntryPtr &entry );
	const EntryPtr get( const EntryPtr &entry );
	void remove();
	void append();
private:
	void newConnCb( const libeva::TcpConnectionPtr &conn );
	void finConnCb( const libeva::TcpConnectionPtr &conn );


	// struct Shard {
	// 	// fine-grained lock
	// 	// std::mutex shardMutex;
	// 	// 使用lazy list构造哈希表
	// 	// std::unordered_set<EntryPtr, detail::EntryPtrHasher, detail::EntryPtrEqual> buckets;
	// 	ds::HashSet<EntryPtr, 1, detail::EntryPtrHasher, detail::EntryPtrEqual> buckets;
	// };

	typedef ds::HashSet<EntryPtr, 1111, detail::EntryPtrCmp, detail::EntryPtrHasher> Shard;
	const static int kNumShards = 4096;

	std::array<std::unique_ptr<Shard>, kNumShards> shards_;

	std::unordered_map<std::string, ConversationPtr> conversations_;
	libeva::TcpServer server;
};


#endif /* _LIBEVA_MINIMEMCACHED_H_ */
