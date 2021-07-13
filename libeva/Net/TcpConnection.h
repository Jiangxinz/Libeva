#ifndef _LIBEVA_NET_TCPCONNECTION_H_
#define _LIBEVA_NET_TCPCONNECTION_H_

#include <memory>
#include <string_view>
#include "libeva/noncopyable.h"
#include "libeva/Net/Socket.h"
#include "libeva/Net/InetAddr.h"
#include "libeva/Net/Buffer.h"
#include "libeva/Callbacks.h"

namespace libeva {

class Reactor;
class Event;

class TcpConnection : noncopyable ,
		public std::enable_shared_from_this<TcpConnection> {
    friend class TcpServer;
public:
    TcpConnection( Reactor* reactor, const std::string &name, Socket sock, InetAddr &peerAddr );
    ~TcpConnection();

    void send( Buffer *buffer );
    void send( const void *data, size_t len );
    
    void setNewConnCb( ConnectionCallback cb )
    { newConnCb_ = cb; }
    void setFinConnCb( ConnectionCallback cb )
    { finConnCb_ = cb; }
    void setMsgCb( MessageCallback cb )
    { msgCb_ = cb; }
    void setCloseCb( CloseCallback cb )
    { closeCb_ = cb; }
    
    // TODO: shutdown

    const char* stateToString() const;
    const std::string &name() const { return name_; }
    Reactor* getReactor() const { return owner_; }
    void connectDestroyed();

    Buffer* outputBuffer()
    { return &outputBuffer_; }
private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void connectEstablished();
    void handleRead( Timestamp receiveTime );
    void handleClose();
    void handleError();
    void setState( StateE state ) { state_ = state; }

    void sendInReactor_strview( const std::string_view& );
    void sendInReactor( const void *data, size_t len );
    Reactor* owner_;
    std::string name_;
    Socket connSock_;
    std::unique_ptr<Event> event_;
    InetAddr peerAddr_;

    ConnectionCallback newConnCb_;
    ConnectionCallback finConnCb_;
    MessageCallback msgCb_;
    CloseCallback closeCb_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;
    StateE state_;
};

}


#endif /* _LIBEVA_NET_TCPCONNECTION_H_ */
