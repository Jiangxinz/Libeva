#include "libeva/Net/TcpServer.h"
#include "libeva/Net/InetAddr.h"
#include "libeva/Callbacks.h"
#include "libeva/Reactor/Reactor.h"
#include "libeva/Net/Buffer.h"
#include "libeva/Timestamp.h"

class EchoServer {
public:
    EchoServer( libeva::Reactor *reactor, libeva::InetAddr &addr) :
        server( reactor, addr ) {
            server.setNewConnCb( std::bind( &EchoServer::newConnCb, this, libeva::_1 ) );
            server.setFinConnCb( std::bind( &EchoServer::finConnCb, this, libeva::_1 ) );
            server.setMsgCb( std::bind( &EchoServer::msgCb, this, libeva::_1, libeva::_2,libeva:: _3 ) );
        }
    
    void start() { server.start(); }

    void setThreadNum( int numThread ) 
    { server.setSubReactorNum( numThread ); }

private:
    void newConnCb( const libeva::TcpConnectionPtr &conn ) {
        printf("New Connection Established\n");
    }

    void finConnCb( const libeva::TcpConnectionPtr &conn ) {
        printf("New Connection destroyed\n");
    }

    void msgCb( const libeva::TcpConnectionPtr& conn, libeva::Buffer* buf, libeva::Timestamp timestamp) {
        std::string content = buf->retrieveAllAsString();
        // printf("received:[%s]\n", content.c_str());
        conn->send( content.c_str(), content.size() );
    }
private:
    libeva::TcpServer server;
};

int
main( int argc, char *argv[] ) {
    libeva::Reactor reactor;
    libeva::InetAddr addr( "127.0.0.1", 8888 );
    EchoServer server( &reactor, addr );
    server.setThreadNum( 3 );
    server.start();
    reactor.dispatch();
    return 0;
}
