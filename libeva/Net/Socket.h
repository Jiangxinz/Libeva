#ifndef _LIBEVA_NET_SOCKET_H_
#define _LIBEVA_NET_SOCKET_H_

#include "libeva/Net/InetAddr.h"
#include "libeva/noncopyable.h"

#include <map>
#include <memory>
#include <functional>
#include <unistd.h>
// FIXME: 
#define REUSE_ADDR 0x01

namespace libeva {

class Socket : noncopyable {
public:
    Socket( int fd ) : pImpl_( std::make_unique<Impl>( fd ) ) {}
    Socket( Socket &&socket ) :
            pImpl_( std::move( socket.pImpl_ ) ) {
            }

    Socket& operator= ( Socket &&socket ) {
        if ( this != &socket ) {
            pImpl_ = std::move( socket.pImpl_ );
        }
        return *this;
    }

    static Socket createListenSocketOn( InetAddr &listenAddr, unsigned opt );

    int fd() const { return pImpl_->fd(); }

    void setOpt( int opt ) { pImpl_->setOpt( opt ); }
    void bind( InetAddr &addr ) { pImpl_->bind( addr ); }
    void listen() { pImpl_->listen(); }
    int accept( struct sockaddr *peerAddr ) { return pImpl_->accept( peerAddr ); }
    Socket accept1( struct sockaddr *peerAddr ) { return pImpl_->accept1( peerAddr ); }
private:
    class Impl {
    public:
        typedef std::function<void(int)> OptFunc;
        Impl( int fd ) : fd_( fd ) {}
        ~Impl() { ::close( fd_ ); }
        void setOpt( int opt );
        void bind( InetAddr &listenAddr );
        void listen();
        int accept( struct sockaddr * peerAddr );
        Socket accept1( struct sockaddr *peerAddr );
        int fd() const { return fd_; }
    private:
        int fd_;
        static std::map<int, OptFunc> optFuncMap_;
    };
    std::unique_ptr<Impl> pImpl_;
};

}

#endif /* _LIBEVA_NET_SOCKET_H_ */