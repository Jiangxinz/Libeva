#ifndef _LIBEVA_TCPSERVER_H_
#define _LIBEVA_TCPSERVER_H_

#include <atomic>
#include <functional>
#include <memory>
#include <map>
#include "libeva/noncopyable.h"
#include "libeva/Net/Socket.h"
#include "libeva/Net/TcpConnection.h"
#include "libeva/Callbacks.h"

namespace libeva {

class Acceptor;
class Reactor;
class ReactorThreadPool;

class TcpServer : public noncopyable {
public:
    TcpServer( Reactor* reactor, InetAddr &addr );
    ~TcpServer();

    void setNewConnCb( const ConnectionCallback & cb )
    { newConnCb_ = cb; }

    void setFinConnCb( const ConnectionCallback & cb )
    { finConnCb_ = cb; }

    void setMsgCb( const MessageCallback & cb )
    { msgCb_ = cb; }

    void setSubReactorNum( int numSubReactor );

    void start();

    Reactor* getSubReactor() const;
private:
    void handleNewConnection( Socket sock, InetAddr &peerAddr );
    void removeConnection( const TcpConnectionPtr &conn );
    void removeConnectionInReactor( const TcpConnectionPtr &conn );
    Reactor *reactor_;
    std::unique_ptr<Acceptor> acceptor_;
    std::unique_ptr<ReactorThreadPool> reactorPool_; 
    
    ConnectionCallback newConnCb_;
    ConnectionCallback finConnCb_;
    MessageCallback msgCb_;
    std::map<std::string, TcpConnectionPtr> connMap_;
    static std::atomic_int numCreated_;
    // std::map<std::string, TcpCon
};

}

#endif /* _LIBEVA_TCPSERVER_H_ */
