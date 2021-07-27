#include <memory>
#include <string.h>
#include <assert.h>
#include "libeva/Net/TcpServer.h"
#include "libeva/Net/Acceptor.h"
#include "libeva/Net/InetAddr.h"
#include "libeva/Reactor/Reactor.h"
#include "libeva/Reactor/ReactorThreadPool.h"
#include "libeva/Logger/Logging.h"

namespace libeva {

void defaultNewConnectionCallback( const TcpConnectionPtr &conn ) {
	printf("defaultNewConnectionCallback\n");

}

void defaultFinConnectionCallback( const TcpConnectionPtr &conn ) {
	printf("defaultFInConnectionCallback\n");
}


void defaultMessageCallback( const TcpConnectionPtr &conn,
				Buffer *buffer,
				Timestamp receiveTime ) {
	printf("defaultMessageCallback\n");
	buffer->retrieveAll();
}


std::atomic_int TcpServer::numCreated_ = 0;

TcpServer::TcpServer( Reactor* reactor, InetAddr &addr ) :
            reactor_( reactor ), acceptor_( std::make_unique<Acceptor>( reactor, addr ) ),
	    reactorPool_( std::make_unique<ReactorThreadPool>( reactor ) ),
       	    newConnCb_( defaultNewConnectionCallback ), 
	    finConnCb_( defaultFinConnectionCallback ),
	    msgCb_( defaultMessageCallback ),
	    connMap_() {
    using std::placeholders::_1;
    using std::placeholders::_2;
    acceptor_->setNewConnCallback( std::bind( &TcpServer::handleNewConnection, this, _1, _2 ) );
}

TcpServer::~TcpServer() {
    reactor_->assertInDispatchingThread();
    for ( auto &conn : connMap_ ) {
	    auto connection = conn.second;
	    conn.second.reset();
	    connection->getReactor()->queueInReactor( 
			    std::bind( &TcpConnection::connectDestroyed, connection ) );
    }
}

void TcpServer::setSubReactorNum( int numSubReactor ) {
	reactorPool_->setThreadNum( numSubReactor );
}

// void ThreadInitCallback()

void TcpServer::start() {
    acceptor_->start();
    reactorPool_->start( nullptr );
}

Reactor* TcpServer::getSubReactor() const {
    return reactorPool_->getNextReactor();
    // return reactor_;
}

void TcpServer::handleNewConnection( Socket sock, InetAddr &peerAddr ) {
    // printf( "recv new connection, fd = %d\n", sock.fd() );
    Reactor *reactor = getSubReactor();

    std::string name( "TcpConnection#" );
    name.append( std::to_string( numCreated_.fetch_add( 1, std::memory_order_relaxed ) ) );
    TcpConnectionPtr newConnection( std::make_shared<TcpConnection>( reactor, name, std::move(sock), peerAddr ) );
    
    newConnection->setNewConnCb( newConnCb_ );
    newConnection->setFinConnCb( finConnCb_ );
    newConnection->setMsgCb( msgCb_ );
    newConnection->setCloseCb( std::bind( &TcpServer::removeConnection, this, _1 ) );
    // printf("name=%s\n", name.c_str());
    assert ( connMap_.find( name ) == connMap_.end() );

    
    connMap_[ name ] = newConnection;

    reactor->runInReactor( std::bind( &TcpConnection::connectEstablished, newConnection ) );
}

void TcpServer::removeConnection( const TcpConnectionPtr &conn ) {
    reactor_->runInReactor( std::bind( &TcpServer::removeConnectionInReactor, this, conn ) );
}

void TcpServer::removeConnectionInReactor( const TcpConnectionPtr &conn ) {
    // printf("TcpServer::removeConnectionInReactor");
    reactor_->assertInDispatchingThread();
    LOG_INFO << "TcpServer::removeConnectionInReactor:  connection " << conn->name();
    assert( connMap_.find( conn->name() ) != connMap_.end() );
    connMap_.erase( conn->name() );
    Reactor *reactor = conn->getReactor();
    reactor->queueInReactor( std::bind( &TcpConnection::connectDestroyed, conn ) );
}

}
