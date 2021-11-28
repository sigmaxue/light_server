#ifndef TASK_H_
#define TASK_H_

#include <global/var.h>

typedef int ( *TaskHandler )( struct Task* );
struct Task {
    int         type;
    int         delay_ms;
    int         init_enable;
    TaskHandler Init;
    int         enable;
    TaskHandler handler;
};

#endif /* TASK_H_ */
