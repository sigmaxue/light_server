#ifndef BASE_EVENT_H_
#define BASE_EVENT_H_

struct BaseEvent* base_event;

typedef void ( *Handler )( struct BaseEvent* base_event );

struct BaseEvent {
    Handler ReadHandler;
    Handler WriteHandler;
    Handler CloseHandler;
    int     fd;
    void*   socket_ptr;
    void*   read_buffer;
    int     read_size;
    int     read_buffer_max_size;
    void*   write_buffer;
    int     write_buffer_max_size;
    int     write_size;
};

#endif /* BASE_EVENT_H_ */
