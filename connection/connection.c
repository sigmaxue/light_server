#include "connection.h"

#include <connection/socket.h>
#include <core/base_event.h>
#include <core/reactor.h>
#include <global/var.h>
#include <sys/epoll.h>
#include <task/task.h>

extern struct Reactor thread_reactor;

int ConnectionClose( struct Task *task ) {
    struct BaseEvent *event  = ( struct BaseEvent * )task->event;
    struct Socket *   socket = ( struct Socket * )event->socket_ptr;

    Close( socket );

    return 0;
}

int CheckComplete( char *buffer, int size ) {
    char *ret = strstr( buffer, "\r\n\r\n" );
    if ( ret == 0 ) {
        return -1;
    }
    return ret - buffer + 4;
}

int ConnectionRecv( struct Task *task ) {
    struct BaseEvent *event  = ( struct BaseEvent * )task->event;
    struct Socket *   socket = ( struct Socket * )event->socket_ptr;

    printf( "%s, fd:%d \n", __FUNCTION__, socket->fd_ );
    if ( socket->fd_ == 0 ) {
        return -1;
    }

    int ret = Read( socket, socket->read_buffer + socket->read_size,
                    socket->read_buffer_max_size - socket->read_size );


    if ( ret == kIOErrorClose || ret == kIOErrorFatal ) {
        Close( socket );
        return -1;
    }

    if ( ret == kIOErrorEAGAIN ) {
        // 空设置epollin
        event->events |= EPOLLIN;
        ret = ModifyEvent( reactor, event, event->events );
        return ret;
    }

    socket->read_size += ret;

    printf( "%s fd: %d %d\n", __FUNCTION__, event->fd, ret );
    // check complete
    if ( ( ret = CheckComplete( socket->read_buffer, socket->read_size ) ) >
         0 ) {
        StartProcess( task, socket->read_buffer, ret );

        socket->read_size -= ret;
        if ( socket->read_size > 0 ) {
            memcpy( socket->read_buffer, socket->read_buffer + ret,
                    socket->read_size );
            memset( socket->read_buffer + socket->read_size, 0, ret );
        }
    }

    return ret;
}


int ConnectionSend( struct Task *task ) {
    struct BaseEvent *event  = ( struct BaseEvent * )task->event;
    struct Socket *   socket = ( struct Socket * )event->socket_ptr;
    if ( socket->fd_ == 0 ) {
        return -1;
    }
    int ret = -1;
    if ( socket->write_size != 0 ) {
        ret = Write( socket, socket->write_buffer, socket->write_size );
        if ( ret == kIOErrorClose || ret == kIOErrorFatal ) {
            Close( socket );
            return ret;
        }

        if ( ret == kIOErrorEAGAIN ) {
            // 空设置epollin
            event->events |= EPOLLOUT;
            ret = ModifyEvent( reactor, event, event->events );
            return ret;
        }
        if ( ret > 0 ) {
            socket->write_size -= ret;
            if ( socket->write_size > 0 ) {
                memcpy( socket->write_buffer, socket->write_buffer + ret,
                        socket->write_size );

                memset( socket->write_buffer + socket->write_size, 0, ret );
            }
        }
        if ( ret == 0 ) {
            return -1;
        }
    }
    printf( "ConnectionSend: fd: %d, %d\n", task->event->fd, ret );
    return ret;
}

int ConnectionWrite( struct Task *task, char *buffer, int size ) {
    struct BaseEvent *event  = ( struct BaseEvent * )task->event;
    struct Socket *   socket = ( struct Socket * )event->socket_ptr;

    memcpy( socket->write_buffer, buffer, size );
    socket->write_size = size;

    int ret = ConnectionSend( task );
    return ret;
}

int StartProcess( struct Task *task, char *buffer, int size ) {
    printf( "%s, fd:%d \n", __FUNCTION__, task->event->fd );
    int ret = strstr( buffer, "open" );
    if ( ret != 0 ) {
        ret = ConnectionWrite( task, kRsp200, strlen( kRsp200 ) );
        if ( ret > 0 ) {
            struct Task *led_task = &( thread_reactor.tasks[ kTypeLed ] );
            led_task->enable      = 1;
            printf( "opened, fd: %d\n", task->event->fd );
            ConnectionClose( task );
        }
    }

    ret = strstr( buffer, "close" );
    if ( ret != 0 ) {
        ret = ConnectionWrite( task, kRsp200, strlen( kRsp200 ) );
        if ( ret > 0 ) {
            struct Task *led_task = &( thread_reactor.tasks[ kTypeLed ] );
            led_task->enable      = 0;

            printf( "closed, fd: %d\n", task->event->fd );
            ConnectionClose( task );
        }
    }
    return 0;
}
