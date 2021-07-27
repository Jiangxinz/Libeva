#ifndef _LIBEVA_NET_INETADDR_H_
#define _LIBEVA_NET_INETADDR_H_

#include <netinet/in.h>
#include "libeva/utility.h"

namespace libeva {
    
class InetAddr {
public:
    InetAddr() = default;
    InetAddr( StringArg ip, uint16_t port );
    struct sockaddr_in* getSocketAddr() { return &addr_; }
private:
    struct sockaddr_in addr_;
};

}

#endif /* _LIBEVA_NET_INETADDR_H_ */