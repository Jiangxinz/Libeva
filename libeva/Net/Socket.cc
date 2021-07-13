#include <sys/types.h>
#include <sys/socket.h>
#include "libeva/Logger/Logging.h"

#include "libeva/Net/Socket.h"

namespace libeva {

namespace {

void setReuseAddr( int fd ) {
    int optval = 1;
    int ret = ::setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
                    &optval, static_cast<socklen_t>( sizeof optval ) );
    if ( ret < 0 ) {
        LOG_FATAL << "set reuseaddr failed";
    }
}

}


std::map<int, Socket::Impl::OptFunc> Socket::Impl::optFuncMap_ {
    { REUSE_ADDR, setReuseAddr }
};

Socket Socket::createListenSocketOn( InetAddr &listenAddr, unsigned opt ) {
    int sockfd = ::socket( AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0 );
    if ( sockfd < 0 ) {
        LOG_FATAL << "socket create error";
    }
    Socket ret( sockfd );
    ret.setOpt( opt );
    ret.bind( listenAddr );
    return ret;
}

void Socket::Impl::setOpt( int opt ) {
    for ( auto &p : optFuncMap_ ) {
        if ( p.first & opt ) {
            p.second( fd_ );
        }
    }
}

void Socket::Impl::bind( InetAddr &listenAddr ) {
    int ret = ::bind( fd_, 
                        reinterpret_cast<struct sockaddr *>( listenAddr.getSocketAddr() ), 
                        static_cast<socklen_t>( sizeof( struct sockaddr_in6 ) ) );
    if (ret < 0) {
        LOG_SYSFATAL << "bind error ";
    }
}

void Socket::Impl::listen() {
    int ret = ::listen( fd_, SOMAXCONN );
    if ( ret < 0 ) {
        LOG_SYSFATAL << "listen error";
    }
}

int Socket::Impl::accept( struct sockaddr * peerAddr ) {
    socklen_t socklen = static_cast<socklen_t>( sizeof( struct sockaddr ) );
    int connfd = ::accept4( fd_, peerAddr,  &socklen, SOCK_NONBLOCK | SOCK_CLOEXEC );
    if (connfd < 0) {
        // 保存当前errno,防止被其它函数的操作覆盖
        int savedErrno = errno;
        LOG_SYSERR << "Socket::accept";
        switch (savedErrno) {
            // 以下错误是非致命错误，应该忽略
            // 比如某个在完成三次握手队列中的socket，
            // 对端已经关闭，这时accept会出错
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO: // ???
            case EPERM:
            case EMFILE: // per-process lmit of open file desctiptor ???
                errno = savedErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
            // unexpected errors
                LOG_FATAL << "unexpected error of ::accept " << savedErrno;
                break;
            default:
                LOG_FATAL << "unknown error of ::accept " << savedErrno;
                break;
        }
    }
    return connfd;
}

Socket Socket::Impl::accept1( struct sockaddr *peerAddr ) {
    socklen_t socklen = static_cast<socklen_t>( sizeof( struct sockaddr ) );
    int connfd = ::accept4( fd_, peerAddr,  &socklen, SOCK_NONBLOCK | SOCK_CLOEXEC );
    return Socket( connfd );
}

}