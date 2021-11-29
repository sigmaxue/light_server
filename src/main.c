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
#include <task/task.h>

#include <pthread.h>
#include <sys/epoll.h>

extern struct Reactor reactor;
extern struct Reactor thread_reactor;

extern int LedInit( struct Task *task );
extern int LedTask( struct Task *task );

struct BaseEvent *led_ev;
void *            ThreadRun( void *arg ) {
    struct Reactor *thread_reactor = ( struct Reactor * )arg;
    RunReactor( thread_reactor );
}

void InitThreadReactor( ) {
    ReactorInit( &thread_reactor );

    struct BaseEvent *task = malloc( sizeof( struct BaseEvent ) );

    task->task.Init            = LedInit;
    task->task.handler         = LedTask;
    task->task.ContinueHandler = LedTask;
    task->task.delay_ms        = 500;
    task->task.enable          = 0;
    task->task.init_enable     = 0;
    task->task.type            = kTypeLed;


    int ret = AddTask( &thread_reactor, task );
    if ( ret < 0 ) {
        return;
    }
    led_ev = task;

    ret = 0;
    pthread_t tid;
    ret = pthread_create( &tid, NULL, ThreadRun, &thread_reactor );
    printf( "pthread_create: %d \n", ret );
}

int main( int argc, char *argv[] ) {
    if ( argc < 2 ) {
        printf( "%s port\n", argv[ 0 ] );
        return -1;
    }

    int listen_port = atoi( argv[ 1 ] );

    InitThreadReactor( );

    int ret = 0;
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
    event.reactor      = &reactor;

    ret =
        AddEvent( &reactor, &event, EPOLLIN | EPOLLET | EPOLLERR | EPOLLRDHUP );
    printf( "AddEvent: %d \n", ret );

    RunReactor( &reactor );

    free( listen );

    return 0;
}
