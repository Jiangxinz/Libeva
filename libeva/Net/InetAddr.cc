#include <string.h>
#include <arpa/inet.h>
#include "libeva/Net/InetAddr.h"
#include "libeva/Logger/Logging.h"

namespace libeva {

InetAddr::InetAddr( StringArg ip, uint16_t port ) {
    memset( &addr_, 0, sizeof( addr_ ) );
    addr_.sin_family = AF_INET;
    if ( inet_pton( AF_INET, ip.c_str(), &addr_.sin_addr ) < 0 ) {
        LOG_FATAL << "inet_pton failed";
    }
    addr_.sin_port = htons( port );
}

}