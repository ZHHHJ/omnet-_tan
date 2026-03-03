#ifndef STATE_H
#define STATE_H

#define DEFINE_STATES \
    X(IDLE)          \
    X(TRAN)       \
    X(HANDING_TRAN)       \
    X(READY_TRAN) \
    X(HANDING_RECV)       \
    X(RECV)    \

enum State {
    #define X(name) name,
    DEFINE_STATES
    #undef X
};

#endif


