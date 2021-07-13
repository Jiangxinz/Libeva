#include <sys/timerfd.h>
#include "libeva/Reactor/Reactor.h"
#include "libeva/Reactor/Event.h"
#include "libeva/Timestamp.h"
#include <string.h>

libeva::Reactor *g_reactor;

void timeout(libeva::Timestamp) {
    printf("Timeout!\n");
    g_reactor->stop();
}

void func2() {
    printf("void func2\n");
}


void func() {
    printf("void func\n");
    g_reactor->runInReactor( func2 );
}

int
main(int argc, char *argv[]) {
    libeva::Reactor reactor;
    g_reactor = &reactor;

    int timerfd = ::timerfd_create( CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC );
    libeva::Event event(&reactor, timerfd);
    event.setReadCallback(timeout);
    event.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    reactor.runInReactor( func );
    reactor.dispatch();

    ::close(timerfd);
}