#include "minimemcached/MiniMemcached.h"
#include "minimemcached/Conversation.h"

const int MiniMemcached::kNumShards;


MiniMemcached::MiniMemcached( libeva::Reactor *reactor, libeva::InetAddr &addr ) :
	server( reactor, addr ) { 
		server.setNewConnCb(
			std::bind( &MiniMemcached::newConnCb, this, libeva::_1 ) 
			);
		server.setFinConnCb(
			std::bind( &MiniMemcached::finConnCb, this, libeva::_1 ) 
			);
		server.setSubReactorNum( 1 );
		EntryPtr greatestKey = std::make_shared<Entry>( std::string(100, 'z') );
		for ( int i = 0; i < kNumShards; ++i ) {
			// FIXME: static 
			shards_[ i ] = std::make_unique<Shard>( greatestKey );
		}
	}

MiniMemcached::~MiniMemcached() { }

void MiniMemcached::set( const EntryPtr &entry) {
	// TODO: hash by key
	// unsigned long idx = detail::EntryPtrHasher()( entry ) % kNumShards;
	unsigned long idx = entry->hash() % kNumShards;
	// printf("set idx = %lu\n", idx);
	Shard* shard = shards_[ idx ].get();
	shard->add( entry );
	// EntryPtr target = shard.get( entryPtr );
}

const EntryPtr MiniMemcached::get( const EntryPtr &entry ) {
	// unsigned long idx = detail::EntryPtrHasher()( entry ) % kNumShards;
	// unsigned long idx = std::hash<EntryPtr>()( entry) % kNumShards;
	unsigned long idx = entry->hash() % kNumShards;
	// printf("get idx = %lu\n", idx);
	Shard* shard = shards_[ idx ].get();
	const EntryPtr &target = shard->get( entry );
	return target;
}

void MiniMemcached::remove() {
	// TODO:
}

void MiniMemcached::append() {
}

void MiniMemcached::dumpShards() {
}

void MiniMemcached::newConnCb( const libeva::TcpConnectionPtr &conn ) {
	const std::string &connName = conn->name();
	assert( conversations.find( connName ) == conversations.end() );
	ConversationPtr newConversation = std::make_unique<Conversation>( this, conn );
	conversations_[ connName ] = std::move( newConversation );
}

void MiniMemcached::finConnCb( const libeva::TcpConnectionPtr &conn ) {
	assert( conversations.find( connName ) != conversations.end() );
	conversations_.erase( conn->name() );
}

