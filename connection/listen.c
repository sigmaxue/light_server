#include "listen.h"

#include <connection/socket.h>
#include <core/base_event.h>
#include <core/reactor.h>
#include <global/var.h>
#include <sys/epoll.h>
#include <task/task.h>



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

    // socket reuse not free
    struct BaseEvent *event =
        ( struct BaseEvent * )malloc( sizeof( struct BaseEvent ) );
    event->reactor                = event_listen->reactor;
    event->ReadHandler            = ReadHandler;
    event->WriteHandler           = WriteHandler;
    event->CloseHandler           = CloseHandler;
    event->socket_ptr             = client;
    event->fd                     = client->fd_;
    client->read_buffer           = malloc( kBufferSize );
    client->read_size             = 0;
    client->read_buffer_max_size  = kBufferSize;
    client->write_buffer          = malloc( kBufferSize );
    client->write_size            = 0;
    client->write_buffer_max_size = kBufferSize;
    event->events                 = EPOLLIN | EPOLLET | EPOLLERR | EPOLLRDHUP;

    int ret = AddEvent( &reactor, event, event->events );
    printf( "AddEvent fd: %d %d\n", event->fd, ret );
}

void ListenWriteHandler( void *ev ) {
    struct BaseEvent *event = ( struct BaseEvent * )ev;
}

void ListenCloseHandler( void *ev ) {
    struct BaseEvent *event = ( struct BaseEvent * )ev;
    Close( event->fd );
    free( event->socket_ptr );
}
