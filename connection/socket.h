#ifndef SOCKET_H_
#define SOCKET_H_
#include <global/var.h>

struct Socket {
    int  fd_;
    char read_buffer[ kBufferSize ];
    int  read_size;
    int  read_buffer_max_size;
    char write_buffer[ kBufferSize ];
    int  write_buffer_max_size;
    int  write_size;
};

void ReadHandler( void* ev );
void WriteHandler( void* ev );
void CloseHandler( void* ev );

int Init( struct Socket* socket );

int  Read( struct Socket* socket, char* buffer, int max_len );
int  Write( struct Socket* socket, char* buffer, int len );
int  Accept( struct Socket* socket );
int  Listen( struct Socket* socket );
int  ListenInit( struct Socket* so, int port );
int  Bind( struct Socket* socket, int port );
void Close( struct Socket* socket );

int HandlePosixIOResult( int ret );



#endif /* SOCKET_H_ */
