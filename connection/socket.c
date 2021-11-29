#include "socket.h"

#include <arpa/inet.h>
#include <connection/connection.h>
#include <core/base_event.h>
#include <errno.h>
#include <global/var.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <task/task.h>

extern int ConnectionRecv( struct Task* task );
extern int ConnectionSend( struct Task* task );



void ReadHandler( void* ev ) {
    struct BaseEvent* event  = ( struct BaseEvent* )ev;
    struct Socket*    socket = ( struct Socket* )event->socket_ptr;

    printf( "%s, fd:%d \n", __FUNCTION__, socket->fd_ );
    event->events |= ~EPOLLIN;
    int ret = ModifyEvent( event->reactor, event, event->events );

    struct Task* task = ( struct Task* )malloc( sizeof( struct Task ) );
    task->event       = event;
    task->handler     = ConnectionRecv;

    ret = AddTask( event->reactor, task );
    if ( ret < 0 ) {
        printf( "ReadHandler AddTask Failed: fd: %d, ret: %d", socket->fd_,
                ret );
    }
}

void WriteHandler( void* ev ) {
    struct BaseEvent* event  = ( struct BaseEvent* )ev;
    struct Socket*    socket = ( struct Socket* )event->socket_ptr;
    int               ret    = 0;

    printf( "%s, fd:%d \n", __FUNCTION__, socket->fd_ );

    event->events |= ~EPOLLOUT;
    ret = ModifyEvent( event->reactor, event, event->events );

    struct Task* task = ( struct Task* )malloc( sizeof( struct Task ) );
    task->event       = event;
    task->handler     = ConnectionSend;

    ret = AddTask( event->reactor, task );
    if ( ret < 0 ) {
        printf( "WriteHandler AddTask Failed: fd: %d, ret: %d", socket->fd_,
                ret );
    }
}

void CloseHandler( void* ev ) {
    struct BaseEvent* event  = ( struct BaseEvent* )ev;
    struct Socket*    socket = ( struct Socket* )event->socket_ptr;

    printf( "%s, fd: %d\n", __FUNCTION__, socket->fd_ );

    Close( socket );
    free( event->socket_ptr );
    free( event );
}

int ListenInit( struct Socket* so, int port ) {
    int ret = 0;


    ret = Init( so );
    if ( ret < 0 )
        return ret;

    ret = Bind( so, port );
    if ( ret < 0 )
        return ret;

    ret = Listen( so );
    if ( ret < 0 )
        return ret;

    return so->fd_;
}

int Init( struct Socket* so ) {
    int fd     = 0;
    int optval = 1; /* listen file(socket) descriptor */

    /* create a socket descriptor */
    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
        return -1; /* error */

    /* eliminates "address already in use" error from bind */
    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, ( const void* )&optval,
                     sizeof( int ) ) < 0 )
        return -1;

    so->fd_ = fd;
    return fd;
}

int Bind( struct Socket* socket, int port ) {
    struct sockaddr_in server_addr;                    /* address of server */
    bzero( &server_addr, sizeof( server_addr ) );      /* fill with 0 */
    server_addr.sin_family      = AF_INET;             /* protocol family */
    server_addr.sin_addr.s_addr = htonl( INADDR_ANY ); /* IP addresss */
    server_addr.sin_port        = htons( ( unsigned short )port ); /* port */
    if ( bind( socket->fd_, ( struct sockaddr* )&server_addr,
               sizeof( server_addr ) ) <
         0 ) /* bind fd_ with the sever's address */
        return -1;
    return 0;
}


int Accept( struct Socket* socket ) {
    struct sockaddr_in client_addr; /* address of server */
    int                addr_len = 0;
    return accept( socket->fd_, ( struct sockaddr* )&client_addr, &addr_len );
}

int Listen( struct Socket* socket ) {
    if ( listen( socket->fd_, 1024 ) < 0 )
        return -1;

    return 0;
}


int Read( struct Socket* socket, char* buffer, int max_len ) {
    int ret = read( socket->fd_, buffer, max_len );
    return HandlePosixIOResult( ret );
}

int Write( struct Socket* socket, char* buffer, int len ) {
    int ret = write( socket->fd_, buffer, len );
    return HandlePosixIOResult( ret );
}

int HandlePosixIOResult( int ret ) {
    if ( ret >= 0 ) {
        return ret;
    }
    if ( errno == EPIPE ) {
        return kIOErrorClose;
    }
    if ( errno == EINTR ) {
        return kIOErrorTryAgain;
    }
    if ( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINPROGRESS ) {
        return kIOErrorEAGAIN;
    }
    return kIOErrorFatal;
}

void Close( struct Socket* socket ) {
    if ( socket->fd_ ) {
        close( socket->fd_ );
    }
    socket->fd_ = 0;
}
