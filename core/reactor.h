#ifndef REACTOR_H_
#define REACTOR_H_

struct BaseEvent;

struct Reactor {
    int epoll_timeout_;
    int epoll_fd_;
    int max_epoll_events_;
};

void ReactorInit( struct Reactor* reactor );

int AddEvent( struct Reactor*   reactor,
              struct BaseEvent* base_event,
              int               events );

int RunReactor( struct Reactor* reactor );


extern struct Reactor reactor;

#endif /* REACTOR_H_ */
