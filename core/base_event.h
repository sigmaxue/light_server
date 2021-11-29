#ifndef BASE_EVENT_H_
#define BASE_EVENT_H_

#include <task/task.h>
struct BaseEvent;
struct Task;

typedef void ( *Handler )( struct BaseEvent* base_event );

struct BaseEvent {
    struct BaseEvent* next;

    Handler ReadHandler;
    Handler WriteHandler;
    Handler CloseHandler;
    int     fd;
    void*   socket_ptr;
    // global arg
    int             events;
    struct Reactor* reactor;
    struct Task     task;
};

#endif /* BASE_EVENT_H_ */
