#include "reactor.h"
#include <core/base_event.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <task/task.h>

struct Reactor reactor;
struct Reactor thread_reactor;

void ReactorInit( struct Reactor* reactor ) {
    reactor->epoll_fd_         = epoll_create( 1024 );
    reactor->epoll_timeout_    = 100;
    reactor->max_epoll_events_ = 1024;
    reactor->task_num          = 0;
}

int ModifyEvent( struct Reactor*   reactor,
                 struct BaseEvent* base_event,
                 int               events ) {
    struct epoll_event epoll_event;
    epoll_event.data.ptr = ( void* )base_event;
    if ( events != 0 ) {
        epoll_event.events = events | EPOLLET;
    } else {
        epoll_event.events =
            EPOLLIN | EPOLLOUT | EPOLLET | EPOLLERR | EPOLLRDHUP;
    }

    int fd = base_event->fd;
    if ( epoll_ctl( reactor->epoll_fd_, EPOLL_CTL_MOD, fd, &epoll_event ) <
         0 ) {
        return -1;
    }
    return 0;
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

int DelEvent( struct Reactor* reactor, struct BaseEvent* base_event ) {
    printf( "Reactor: %s\n", __FUNCTION__ );
    struct epoll_event epoll_event;
    epoll_event.data.ptr = ( void* )base_event;

    int fd = base_event->fd;
    if ( epoll_ctl( reactor->epoll_fd_, EPOLL_CTL_DEL, fd, &epoll_event ) <
         0 ) {
        return -1;
    }

    return 0;
}

int AddTask( struct Reactor* reactor, struct BaseEvent* task ) {
    if ( reactor->task_num >= kReactorMaxTaskNum ) {
        return -1;
    }

    if ( reactor->tasks_tail == 0 ) {
        reactor->tasks_tail = task;
        reactor->tasks_head = task;
    } else {
        reactor->tasks_tail->next = task;
        reactor->tasks_tail       = task;
    }
    reactor->task_num++;

    return 0;
}


int InitTasks( struct Reactor* reactor ) {
    struct BaseEvent* handle = reactor->tasks_head;
    while ( handle ) {
        handle->task.Init( handle );
        handle = handle->next;
    }

    return 0;
}

int RunTasks( struct Reactor* reactor ) {
    int ret = 0;
    int i   = 0;

    struct BaseEvent* handle = reactor->tasks_head;
    struct BaseEvent* tmp;
    for ( i = 0; i < kReactorMaxTaskNum; i++ ) {
        if ( !handle ) {
            break;
        }
        tmp                 = handle;
        handle              = handle->next;
        reactor->tasks_head = handle;
        reactor->task_num--;

        ret = tmp->task.handler( handle );
        if ( ret == kTaskContinue ) {
            if ( tmp->task.ContinueHandler )
                tmp->task.ContinueHandler( tmp );
        }

        if ( ret == kTaskFinish ) {
            if ( tmp->task.FinishHandler ) {
                tmp->task.FinishHandler( tmp );
            } else {
                free( tmp );
            }
        }
    }

    return 0;
}

int RunReactor( struct Reactor* reactor ) {
    printf( "Reactor: %s\n", __FUNCTION__ );
    struct epoll_event* events = ( struct epoll_event* )malloc(
        sizeof( struct epoll_event ) * reactor->max_epoll_events_ );

    InitTasks( reactor );
    while ( 1 ) {
        RunTasks( reactor );

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

    free( events );
    return 0;
}
