#ifndef REACTOR_H_
#define REACTOR_H_

#include <global/var.h>

struct BaseEvent;

struct Reactor {
    int epoll_timeout_;
    int epoll_fd_;
    int max_epoll_events_;

    struct Task* tasks;
    int          task_num;
};

void ReactorInit( struct Reactor* reactor );

int AddEvent( struct Reactor*   reactor,
              struct BaseEvent* base_event,
              int               events );

int InitTasks( struct Reactor* reactor );
int RunTasks( struct Reactor* reactor );
int RunReactor( struct Reactor* reactor );


extern struct Reactor reactor;
extern struct Reactor thread_reactor;

#endif /* REACTOR_H_ */
