#ifndef LISTEN_H_
#define LISTEN_H_


void ListenReadHandler( void *event );

void ListenWriteHandler( void *event );

void ListenCloseHandler( void *event );


#endif /* LISTEN_H_ */
