#include "libeva/Net/TcpConnection.h"
#include "libeva/Reactor/Reactor.h"
#include "libeva/Reactor/Event.h"
#include "libeva/Logger/Logging.h"

namespace libeva {

TcpConnection::TcpConnection( Reactor *reactor, const std::string &name, 
		Socket sock, InetAddr &peerAddr ) :
                owner_( reactor ), name_( name ), connSock_( std::move( sock ) ), 
                event_( std::make_unique<Event>( reactor, connSock_.fd() ) ), 
                peerAddr_( peerAddr ), state_( kDisconnected ) {
		    using std::placeholders::_1;
                    event_->setReadCallback( std::bind( &TcpConnection::handleRead, this, _1 ) );
		    event_->setWriteCallback(
				    std::bind( &TcpConnection::handleWrite, this ) );
		    event_->setCloseCallback(
				    std::bind( &TcpConnection::handleClose, this ) );
		    event_->setErrorCallback(
				    std::bind( &TcpConnection::handleError, this ) );

}

TcpConnection::~TcpConnection() {

}

void TcpConnection::send( Buffer *buf ) {
	// FIXME:: try to write buf into socket directly
     if (state_ == kConnected)
  {
    if (owner_->isInReactorThread())
    {
      sendInReactor(buf->peek(), buf->readableBytes());
      buf->retrieveAll();
    }
    else
    {
 void (TcpConnection::*fp)(const std::string_view& message) = &TcpConnection::sendInReactor_strview;
       owner_->runInReactor(
           std::bind(fp,
                     this,     // FIXME
                     buf->retrieveAllAsString()));
    }
  }
}

void TcpConnection::send( const void *data, size_t len ) {
	// owner_->runInReactor( std::bind( &TcpConnection::sendInReactor, this, data, len ) );
	if ( state_ == kConnected ) {
		if ( owner_->isInReactorThread() ) {
			sendInReactor( data, len );
		} else {
       owner_->runInReactor(
           std::bind(&TcpConnection::sendInReactor,
                     this,     // FIXME
                     data, len));
		}

	}
	
}

void TcpConnection::sendInReactor_strview( const std::string_view &str ) {
	sendInReactor( str.data(), str.size() );
}

void TcpConnection::sendInReactor( const void *data, size_t len ) {
	// printf("TcpConnection::sendInReactor\n");
	if ( state_ == kDisconnected ) {
		LOG_WARN << "Connection is closed, give up writing\n";
		return;
	}
	ssize_t numWrite = 0;
	size_t remaining = len;
	bool intoleranceError = false;
	if (!event_->isWriting() && outputBuffer_.readableBytes() == 0) {
		numWrite = ::write( connSock_.fd(), data, len );
		if ( numWrite >= 0 ) {
			remaining -= numWrite;
			// TODO: lowWaterMarkCallbak
		} else {
			numWrite = 0;
			if ( errno != EWOULDBLOCK ) {
				LOG_SYSERR << "TcpConnection::sendInReactor";
				if ( errno == EPIPE || errno == ECONNRESET ) {
					intoleranceError = true;
				}
			}
		}
	}
	assert ( remaining <= len );
	if ( !intoleranceError && remaining > 0 ) {
		// TODO: highWaterMarkCallback
		outputBuffer_.append( static_cast<const char *>( data )+numWrite, remaining );
		if ( !event_->isWriting() ) {
      			event_->enableWriting();
		}
	}
}

void TcpConnection::connectEstablished() {
	event_->enableReading();
	setState( kConnected );
	newConnCb_( shared_from_this() );
}

void TcpConnection::handleRead( Timestamp receiveTime ) {
	owner_->assertInDispatchingThread();
	// printf("TcpConnection::handleRead: %s\n", receiveTime.toFormattedString().c_str());
	int savedErrno = 0;
	ssize_t n = inputBuffer_.readFd( event_->fd(), &savedErrno );
	if ( n > 0 ) {
		msgCb_( shared_from_this(), &inputBuffer_, receiveTime );
	} else if ( n == 0 ) {
		handleClose();
	} else {
		errno = savedErrno;
    		LOG_SYSERR << "TcpConnection::handleRead";
    		handleError();
	}
}

void TcpConnection::handleWrite() {
	owner_->assertInDispatchingThread();
	if ( event_->isWriting() ) {
		ssize_t n = ::write( event_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes() );
		if ( n > 0 ) {
			outputBuffer_.retrieve( n );
			if ( outputBuffer_.readableBytes() == 0 ) {
				event_->disableWriting();
				// TODO: low water call back
				if ( state_ == kDisconnecting ) {
					shutdownInLoop();
				}
			}
		} else {
			LOG_SYSERR << "TcpConnection::handleWrite";
		}
	} else {
		LOG_TRACE << "Connection " << event_->fd()
			<< " has no more writing";
	}
}

void TcpConnection::shutdownInLoop() {
	owner_->assertInDispatchingThread();
	if ( !event_->isWriting() ) {
		::shutdown( event_->fd(), SHUT_WR );
	}
}

void TcpConnection::handleClose() {
	owner_->assertInDispatchingThread();
  	LOG_TRACE << "fd = " << event_->fd() << " state = " << stateToString();
  	assert( state_ == kConnected || state_ == kDisconnecting );
  	setState( kDisconnected );
  	event_->disableAll();

  	TcpConnectionPtr guardThis( shared_from_this() );
  	finConnCb_( guardThis );
  	closeCb_( guardThis );
}

void TcpConnection::handleError() {
       int optval;
       socklen_t optlen = static_cast<socklen_t>(sizeof optval);
       int err;
       if (::getsockopt(event_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
       {
         err = errno;
       }
       else
       {
         err = optval;
       }
       (void) err;
}

const char* TcpConnection::stateToString() const
{
  switch (state_)
  {
    case kDisconnected:
      return "kDisconnected";
    case kConnecting:
      return "kConnecting";
    case kConnected:
      return "kConnected";
    case kDisconnecting:
      return "kDisconnecting";
    default:
      return "unknown state";
  }
}

void TcpConnection::connectDestroyed( ) {
	LOG_TRACE << "TcpConnection::connectDestroyed";
	owner_->assertInDispatchingThread();
	if ( state_ == kConnected ) {
		setState( kDisconnected );
		event_->disableAll();
		finConnCb_( shared_from_this() );
	}
	event_->remove();
}

}
