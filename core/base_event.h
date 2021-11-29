#ifndef BASE_EVENT_H_
#define BASE_EVENT_H_

struct BaseEvent;

typedef void ( *Handler )( struct BaseEvent* base_event );

struct BaseEvent {
    Handler ReadHandler;
    Handler WriteHandler;
    Handler CloseHandler;
    int     fd;
    void*   socket_ptr;
    // global arg
    int             events;
    struct Reactor* reactor;
};

#endif /* BASE_EVENT_H_ */
