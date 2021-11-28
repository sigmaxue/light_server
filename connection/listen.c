#include "listen.h"

#include <connection/socket.h>
#include <core/base_event.h>
#include <core/reactor.h>
#include <global/var.h>
#include <sys/epoll.h>



void ReadHandler( void *ev ) {
    printf( "%s\n", __FUNCTION__ );
    struct BaseEvent *event  = ( struct BaseEvent * )ev;
    struct Socket *   socket = ( struct Socket * )event->socket_ptr;
    int               ret = Read( socket, event->read_buffer + event->read_size,
                    event->read_buffer_max_size );

    printf( "socket: %d, read_size: %d\n", socket->fd_,ret);
    if ( ret < 0 ) {
        return;
    }

    event->read_size = ret;

    ret = strstr( event->read_buffer, "\r\n\r\n" );
    if ( ret == 0 ) {
        return;
    }

    ret = strstr( event->read_buffer, "open" );
    if ( ret != 0 ) {
        printf( "open\n" );

        memcpy( event->write_buffer, kRsp200, strlen( kRsp200 ) );
        event->write_size = strlen( kRsp200 );
        return;
    }
    ret = strstr( event->read_buffer, "close" );
    if ( ret != 0 ) {
        printf( "close\n" );

        memcpy( event->write_buffer, kRsp200, strlen( kRsp200 ) );
        event->write_size = strlen( kRsp200 );
        return;
    }
}

void WriteHandler( void *ev ) {
    printf( "%s\n", __FUNCTION__ );
    struct BaseEvent *event  = ( struct BaseEvent * )ev;
    struct Socket *   socket = ( struct Socket * )event->socket_ptr;
    int               ret    = 0;

    ret = Write( socket, event->write_buffer, event->write_size );
    Close( socket );
}

void CloseHandler( void *ev ) {
    struct BaseEvent *event = ( struct BaseEvent * )ev;
    free( event->socket_ptr );
}


void ListenReadHandler( void *ev ) {
    printf( "%s\n", __FUNCTION__ );
    struct BaseEvent *event_listen = ( struct BaseEvent * )ev;
    struct Socket *   listen = ( struct Socket * )event_listen->socket_ptr;

    int fd = Accept( listen );
    if ( fd < 0 ) {
        return;
    }

    struct Socket *client =
        ( struct Socket * )malloc( sizeof( struct Socket ) );
    client->fd_ = fd;

    printf( "Accept fd:%d\n", client->fd_ );
    struct BaseEvent event;
    event.ReadHandler           = ReadHandler;
    event.WriteHandler          = WriteHandler;
    event.CloseHandler          = CloseHandler;
    event.socket_ptr            = client;
    event.fd                    = client->fd_;
    event.read_buffer           = malloc( kBufferSize );
    event.read_size             = 0;
    event.read_buffer_max_size  = kBufferSize;
    event.write_buffer          = malloc( kBufferSize );
    event.write_size            = 0;
    event.write_buffer_max_size = kBufferSize;

    int ret = AddEvent( &reactor, &event, EPOLLIN | EPOLLOUT );
    printf("AddEvent: %d\n", ret);
}

void ListenWriteHandler( void *ev ) {
    struct BaseEvent *event = ( struct BaseEvent * )ev;
}

void ListenCloseHandler( void *ev ) {
    struct BaseEvent *event = ( struct BaseEvent * )ev;
    Close( event->fd );
    free( event->socket_ptr );
}
