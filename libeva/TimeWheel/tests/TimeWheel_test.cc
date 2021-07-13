#include "libeva/Reactor/Reactor.h"
#include "libeva/TimeWheel/TimeWheel.h"
#include "libeva/Logger/Logging.h"
#include "libeva/TimeWheel/Timer.h"
#include "libeva/Timestamp.h"

libeva::Reactor *g_reactor;
libeva::TimeWheel *g_timwheel;
libeva::Timer *g_timer;

void timeout() {
    LOG_DEBUG << "Time out";
    // g_timwheel->delTimer(g_timer);
}

void timeout2() {
    LOG_DEBUG << "Time out22222";
}

int
main( int argc, char *argv[] ) {
    /*
    libeva::Reactor reactor;
    g_reactor = &reactor;
    libeva::TimeWheel wheel( &reactor );
    g_timwheel = &wheel;
    libeva::Timestamp time(addTime(libeva::Timestamp::now(), 1.0));
    g_timer = wheel.addTimer( timeout, time, 1.25 );
    wheel.addTimer( timeout2, time, 0.5 );
    wheel.addTimer( timeout2, time, 0 );

    reactor.dispatch();

    // delete timer;
*/
    return 0;
}