#include <arpa/inet.h>
#include <connection/listen.h>
#include <connection/socket.h>
#include <core/base_event.h>
#include <core/reactor.h>
#include <malloc.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

#include <sys/epoll.h>

extern struct Reactor reactor;

int main( int argc, char *argv[] ) {
    int listen_port = 8081;

    ReactorInit( &reactor );
    struct Socket *listen =
        ( struct Socket * )malloc( sizeof( struct Socket ) );
    int ret = ListenInit( listen, listen_port );
    printf( "ListenInit: %d \n", ret );

    struct BaseEvent event;
    event.ReadHandler  = ListenReadHandler;
    event.CloseHandler = ListenCloseHandler;
    event.socket_ptr   = listen;
    event.fd           = listen->fd_;

    ret = AddEvent( &reactor, &event, EPOLLIN );
    printf( "AddEvent: %d \n", ret );

    RunReactor( &reactor );

    free( listen );
    return 0;
}
