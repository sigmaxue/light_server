#include "reactor.h"
#include <core/base_event.h>
#include <stdlib.h>
#include <sys/epoll.h>

struct Reactor reactor;

void ReactorInit( struct Reactor* reactor ) {
    reactor->epoll_fd_         = epoll_create( 1024 );
    reactor->epoll_timeout_    = 100;
    reactor->max_epoll_events_ = 1024;
}

int AddEvent( struct Reactor*   reactor,
              struct BaseEvent* base_event,
              int               events ) {
    printf( "Reactor: %s\n", __FUNCTION__ );
    struct epoll_event epoll_event;
    epoll_event.data.ptr = ( void* )base_event;
    if ( events != 0 ) {
        epoll_event.events = events | EPOLLET;
    } else {
        epoll_event.events =
            EPOLLIN | EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP;
    }

    int fd = base_event->fd;
    if ( epoll_ctl( reactor->epoll_fd_, EPOLL_CTL_ADD, fd, &epoll_event ) <
         0 ) {
        return -1;
    }

    return 0;
}

int RunReactor( struct Reactor* reactor ) {
    printf( "Reactor: %s\n", __FUNCTION__ );
    struct epoll_event* events = ( struct epoll_event* )malloc(
        sizeof( struct epoll_event ) * reactor->max_epoll_events_ );

    while ( 1 ) {
        int i = 0;
        int ret =
            epoll_wait( reactor->epoll_fd_, events, reactor->max_epoll_events_,
                        reactor->epoll_timeout_ );
        for ( i = 0; i < ret; i++ ) {
            struct BaseEvent* event = events[ i ].data.ptr;

            if ( events[ i ].events & EPOLLIN ) {
                event->ReadHandler( event );
            }

            if ( events[ i ].events & EPOLLOUT ) {
                event->WriteHandler( event );
            }

            if ( events[ i ].events & EPOLLRDHUP ) {
                event->CloseHandler( event );
            }
        }
    }

    free(events);
    return 0;
}
