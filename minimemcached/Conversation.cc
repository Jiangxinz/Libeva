#include <unistd.h>
#include <iostream>
#include "libeva/Net/TcpConnection.h"
#include "libeva/Logger/Logging.h"
#include "minimemcached/Conversation.h"
// #include "minimemcached/"


Conversation::Conversation( MiniMemcached *server, const libeva::TcpConnectionPtr &conn ) :
	server_( server ),
	conn_( conn ),
	controller_(),
	currEntry_( std::make_shared<Entry>( "Invalid", 128 ) ),
	command_(),
	state_( kNewCommand ) {
		conn_->setMsgCb(
			std::bind( &Conversation::msgCb, this, libeva::_1, libeva::_2, libeva::_3 )
			);
		controller_.insert( std::make_pair( "Test", std::bind( &Conversation::TestHandler, this, libeva::_1, libeva::_2 ) ) );
		controller_.insert( std::make_pair( "get", std::bind( &Conversation::GetHandler, this, libeva::_1, libeva::_2 ) ) );
		controller_.insert( std::make_pair( "set", std::bind( &Conversation::SetHandler, this, libeva::_1, libeva::_2 ) ) );
	}

Conversation::~Conversation() {
}

void Conversation::TestHandler( StringTokenizer &tokenizer, void *arg ) {
	while ( tokenizer.hasMoreToken() ) {
		std::string_view token = tokenizer.nextToken();
		conn_->send( token.data(), token.size() );
		conn_->send( " ",  1 );
	}
	conn_->send( "\r\n",  2 );
}

// static std::string nposStr( 128, 'z' );

void Conversation::GetHandler( StringTokenizer &tokenizer, void *arg ) {
	std::string_view token =  tokenizer.nextToken();
	if ( token.empty() ) {
		printf("Empty Token\n");
		return;
	}
	currEntry_->resetKey( token );
	EntryPtr entry = server_->get( currEntry_ );
	// std::cout << "entry->key[" ;
	// std::cout << entry->key() ;
	// std::cout << "]\n";
	if ( entry ) {
		entry->copyToBuffer( &outputBuf_ );
	}
	outputBuf_.append( "END\r\n" );
        if (conn_->outputBuffer()->writableBytes() > 65536 + outputBuf_.readableBytes()) {
		LOG_DEBUG << "shrink output buffer from " << conn_->outputBuffer()->internalCapacity();
		conn_->outputBuffer()->shrink(65536 + outputBuf_.readableBytes());
	}
	conn_->send( &outputBuf_ );
}

void Conversation::SetHandler( StringTokenizer &tokenizer, void *arg ) {
	std::string_view token =  tokenizer.nextToken();
	if ( !tokenizer.hasMoreToken() ) {
		state_ = kNewCommand;
		return;
	}
	tokenizer.nextToken();
	if ( !tokenizer.hasMoreToken() ) {
		state_ = kNewCommand;
		return;
	}
	tokenizer.nextToken();
	if ( !tokenizer.hasMoreToken() ) {
		state_ = kNewCommand;
		return;
	}
	std::string_view bytes = tokenizer.nextToken();
	int numBytes = atoi( bytes.data() );
	if ( numBytes <= 0 || numBytes > 1024 ) {
		printf("Invalid numBytes = %d\n", numBytes);
		return;
	}
	// currEntry_ = std::make_shared<Entry>( token, numBytes  );
	currEntry_->resetKey( token, numBytes );
	state_ = kReceiveValue;
}

void Conversation::msgCb( const libeva::TcpConnectionPtr &conn, libeva::Buffer* buf, libeva::Timestamp stamp ) {
	while ( buf->readableBytes() > 0 ) {
		// printf("readableBytes=%ld\n", buf->readableBytes());
		// printf("[%s]\n", buf->peek());
		if ( state_ == kNewCommand ) {
			bool isBinary = isBinaryProtocol( buf->peek()[0] );
			assert ( !isBinary ); (void) isBinary;
			const char *crlf = buf->findCRLF();
			if ( crlf ) {
				int len = static_cast<int>( crlf-buf->peek() );
				if ( len != 0 ) {
					std::string_view req( buf->peek(), len );
					handle_command( req );
				}
				buf->retrieve( len+2 );
			} else {
				const char *lf = buf->findEOL();
				if ( lf ) {
					int len = static_cast<int>( lf-(buf->peek()) );
					if ( len != 0 ) {
						std::string_view req( buf->peek(), len );
						handle_command( req );
					}
					buf->retrieve( len+1 );
				} else {
					// TODO: if buf->readableBytes() > 1024 then shutdown connection;
					break;
				}
			}
		} else if ( state_ == kReceiveValue ) {
			receiveValue( buf );
		} else if ( state_ == kDiscardValue ) {
		} else {
			break;
		}
	}
}

void Conversation::receiveValue( libeva::Buffer* buf ) {
	// TODO: handle some error: such as large value
	// printf("currEntry_->numFree()=%lu\n", currEntry_->numFree());
	// sleep(10);
	size_t numBytes = std::min( buf->readableBytes(), currEntry_->numFree() );
	std::string_view value( buf->peek(), numBytes );
	currEntry_->append( buf->peek(), numBytes );
	buf->retrieve( numBytes );
	EntryPtr entry = std::make_shared<Entry>( currEntry_->data(), currEntry_->keyLen(), currEntry_->valueLen(), currEntry_->numFree() ); 
	// std::cout << "receiveValue: entry->key:[" << entry->key() << "]\n";
	server_->set( std::move( entry ) );
	reply("STORED\r\n");
	state_ = kNewCommand;
	// reset
	resetRequest();
}

bool Conversation::handle_command( const std::string_view req ) {
	auto sep = [&]( char ch ) {
		return ch == ' ' ? true : false;
	};
	StringTokenizer tokenizer( req, sep );
	if ( !tokenizer.hasMoreToken() ) {
		reply("Error\r\n");
		return true;
	}
	command_ = tokenizer.nextToken();
	// FIXME: use std::string
	auto it = controller_.find( command_ );
	if ( it == controller_.end() ) {
		printf("---------\n");
		printf("Invalid command:[%s]\r\n", command_.c_str());
		sleep(10);
		reply("Invalid command:[%s]\r\n");
		return true;
	}

	it->second( tokenizer, nullptr );

	return true;
}

void Conversation::resetRequest() {
	command_.clear();
	currEntry_->Reset();
}

void Conversation::reply( std::string_view msg ) {
	conn_->send( msg.data(), msg.size() );
}

std::vector<std::string_view> Conversation::splitBySpace( const std::string_view str ) {
	assert ( false );
	std::vector<std::string_view> vec;
	return vec;
}
