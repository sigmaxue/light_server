#include <arpa/inet.h>
#include <connection/listen.h>
#include <connection/socket.h>
#include <core/base_event.h>
#include <core/reactor.h>
#include <led/led.h>
#include <malloc.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

#include <sys/epoll.h>
#include <pthread.h>

extern struct Reactor reactor;

int main( int argc, char *argv[] ) {
	if (argc<2) {
		printf("%s port\n", argv[0]);
		return -1;
	}
    int listen_port = atoi(argv[1]);
    int ret = 0;

    pthread_t tid;
    ret = pthread_create(&tid, NULL, led_loop, NULL);
    printf( "pthread_create: %d \n", ret );

    ReactorInit( &reactor );
    struct Socket *listen =
        ( struct Socket * )malloc( sizeof( struct Socket ) );
    ret = ListenInit( listen, listen_port );
    printf( "ListenInit: %d \n", ret );

    struct BaseEvent event;
    event.ReadHandler  = ListenReadHandler;
    event.CloseHandler = ListenCloseHandler;
    event.socket_ptr   = listen;
    event.fd           = listen->fd_;

    ret = AddEvent( &reactor, &event, EPOLLIN | EPOLLET );
    printf( "AddEvent: %d \n", ret );

    RunReactor( &reactor );

    free( listen );
    return 0;
}
