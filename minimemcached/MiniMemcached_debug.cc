#include "libeva/Reactor/Reactor.h"
#include "libeva/Net/InetAddr.h"
#include "MiniMemcached.h"

int
main(int argc, char *argv[]) {
	libeva::Reactor reactor;
	libeva::InetAddr addr( "127.0.0.1", 11211 );
	MiniMemcached server( &reactor, addr );
	server.start();

	reactor.dispatch();
}
