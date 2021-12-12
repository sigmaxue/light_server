#include "softPwm.h"
#include "stdio.h"
#include "wiringPi.h"
#include <led/led.h>

#include <core/base_event.h>
#include <global/var.h>
#include <task/task.h>

const int kRedLedPin   = 0;
const int kGreenLedPin = 1;

void InitRedLed( ) {
    softPwmCreate( kRedLedPin, 0, 100 );
    softPwmCreate( kGreenLedPin, 0, 100 );
}

void SetLedColor( char r, char g ) {
    softPwmWrite( kRedLedPin, r );
    softPwmWrite( kGreenLedPin, g );
}

int LedContinueTask( struct BaseEvent *event ) {
    int ret = AddTask( event->reactor, event );
    if ( ret < 0 ) {
        printf( "ReadHandler AddTask Failed: fd: %d, ret: %d", socket->fd_,
                ret );
    }
    return ret;
}

int LedInit( struct BaseEvent *task ) {
    if ( wiringPiSetup( ) == -1 ) {
        printf( "init error" );
        return -1;
    }

    InitRedLed( );

    task->task.type        = kTypeLed;
    task->task.init_enable = 1;

    printf( "%s", __FUNCTION__ );
    return 1;
}


int LedTask( struct BaseEvent *task ) {
    if ( !task->task.init_enable )
        return kTaskFinish;
    if ( !task->task.enable )
        return kTaskContinue;

    SetLedColor( 0xff, 0x00 );
    delay( task->task.delay_ms );
    SetLedColor( 0x00, 0xff );
    delay( task->task.delay_ms );
    SetLedColor( 0xff, 0x45 );
    delay( task->task.delay_ms );
    SetLedColor( 0xff, 0xff );
    delay( task->task.delay_ms );
    SetLedColor( 0x7c, 0xfc );
    delay( task->task.delay_ms );
    SetLedColor( 0x00, 0x00 );
    delay( task->task.delay_ms );

    return kTaskContinue;
}
