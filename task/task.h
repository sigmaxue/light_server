#ifndef TASK_H_
#define TASK_H_

#include <global/var.h>
struct BaseEvent;

typedef int ( *TaskHandler )( struct BaseEvent* );
struct Task {
    struct BaseEvent* event;

    int         type;
    int         delay_ms;
    int         init_enable;
    TaskHandler Init;
    int         enable;
    TaskHandler handler;
    TaskHandler ReadHandler;
    TaskHandler WriteHandler;
    TaskHandler FinishHandler;
    TaskHandler ContinueHandler;
    int         task_state;
};

#endif /* TASK_H_ */
