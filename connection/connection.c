#include "connection.h"

#include <connection/socket.h>
#include <core/base_event.h>
#include <core/reactor.h>
#include <global/var.h>
#include <sys/epoll.h>
#include <task/task.h>

extern struct Reactor    thread_reactor;
extern struct BaseEvent *led_ev;

int CheckComplete( char *buffer, int size ) {
    char *ret = strstr( buffer, "\r\n\r\n" );
    if ( ret == 0 ) {
        return -1;
    }
    return ret - buffer + 4;
}

int ConnectionClose( struct BaseEvent *event ) {
    event->task.ReadHandler  = 0;
    event->task.WriteHandler = 0;
    if ( event->task.task_state == kTaskFinish ) {
        event->task.FinishHandler = 0;
        return kTaskFinish;
    }
    struct Socket *socket = ( struct Socket * )event->socket_ptr;
    DelEvent( event->reactor, event );
    if ( socket ) {
        Close( socket );
        event->socket_ptr = 0;
    }
    event->task.task_state    = kTaskFinish;
    event->task.FinishHandler = 0;
    return kTaskContinue;
}

int ContinueRun( struct BaseEvent *event ) {
    struct Socket *socket = ( struct Socket * )event->socket_ptr;
    if ( event->task.task_state == kTaskClose ) {
        event->task.handler = ConnectionClose;
    }

    int ret = AddTask( event->reactor, event );
    if ( ret < 0 ) {
        printf( "ReadHandler AddTask Failed: fd: %d, ret: %d", socket->fd_,
                ret );
    }
    return 0;
}

int ConnectionRecv( struct BaseEvent *event ) {
    if ( event->task.task_state == kTaskClose ||
         kTaskFinish == event->task.task_state ) {
        return kTaskHolding;
    }

    struct Socket *socket = ( struct Socket * )event->socket_ptr;

    printf( "%s, fd:%d \n", __FUNCTION__, socket->fd_ );
    if ( socket->fd_ == 0 ) {
        event->task.task_state = kTaskClose;
        return kTaskContinue;
    }

    int ret = 0;
    do {
        ret = Read( socket, socket->read_buffer + socket->read_size,
                    socket->read_buffer_max_size - socket->read_size );

        if ( ret == kIOErrorClose || ret == kIOErrorFatal ) {
            event->task.task_state = kTaskClose;
            return kTaskContinue;
        }

        if ( ret == kIOErrorEAGAIN ) {
            // 空设置epollin
            event->events |= EPOLLIN;
            ret = ModifyEvent( reactor, event, event->events );
            return kTaskHolding;
        }

        socket->read_size += ret;

        printf( "%s fd: %d %d\n", __FUNCTION__, event->fd, ret );
    } while (
        ( ret = CheckComplete( socket->read_buffer, socket->read_size ) ) < 0 );

    // check complete
    StartProcess( event, socket->read_buffer, ret );

    socket->read_size -= ret;
    if ( socket->read_size > 0 ) {
        memcpy( socket->read_buffer, socket->read_buffer + ret,
                socket->read_size );
        memset( socket->read_buffer + socket->read_size, 0, ret );
    }

    return kTaskContinue;
}



int ConnectionSend( struct BaseEvent *event ) {
    if ( event->task.task_state == kTaskClose ||
         kTaskFinish == event->task.task_state ) {
        return kTaskHolding;
    }

    struct Socket *socket = ( struct Socket * )event->socket_ptr;
    if ( socket->fd_ == 0 ) {
        event->task.task_state = kTaskClose;
        return kTaskContinue;
    }

    int ret = -1;
    while ( socket->write_size != 0 ) {
        ret = Write( socket, socket->write_buffer, socket->write_size );
        if ( ret == kIOErrorClose || ret == kIOErrorFatal ) {
            event->task.task_state = kTaskClose;
            return kTaskContinue;
        }

        if ( ret == kIOErrorEAGAIN ) {
            event->events |= EPOLLOUT;
            ret = ModifyEvent( reactor, event, event->events );
            return kTaskHolding;
        }
        if ( ret > 0 ) {
            socket->write_size -= ret;
            if ( socket->write_size > 0 ) {
                memcpy( socket->write_buffer, socket->write_buffer + ret,
                        socket->write_size );

                memset( socket->write_buffer + socket->write_size, 0, ret );
            }
        } else if ( ret == 0 ) {
            event->task.task_state = kTaskClose;
        }
    }
    return kTaskContinue;
}

int ConnectionWrite( struct BaseEvent *event, char *buffer, int size ) {
    struct Socket *socket = ( struct Socket * )event->socket_ptr;

    memcpy( socket->write_buffer, buffer, size );
    socket->write_size = size;

    return ConnectionSend( event );
}

int StartProcess( struct BaseEvent *event, char *buffer, int size ) {
    printf( "%s, fd:%d \n", __FUNCTION__, event->fd );
    int ret = strstr( buffer, "open" );
    if ( ret != 0 ) {
        ret = ConnectionWrite( event, kRsp200, strlen( kRsp200 ) );
        if ( ret != kTaskHolding ) {
            led_ev->task.enable = 1;
            printf( "opened, fd: %d\n", event->fd );
            event->task.task_state = kTaskClose;
        }
    }

    ret = strstr( buffer, "close" );
    if ( ret != 0 ) {
        ret = ConnectionWrite( event, kRsp200, strlen( kRsp200 ) );
        if ( ret != kTaskHolding ) {
            led_ev->task.enable = 1;
            printf( "closed, fd: %d\n", event->fd );
            event->task.task_state = kTaskClose;
        }
    }
    return kTaskContinue;
}
