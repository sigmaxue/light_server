#include "led.h"
#include "softPwm.h"
#include "stdio.h"
#include "wiringPi.h"

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

int LedInit( struct Task *task ) {
    if ( wiringPiSetup( ) == -1 ) {
        printf( "init error" );
        return -1;
    }

    InitRedLed( );

    task->type        = kTypeLed;
    task->init_enable = 1;

    printf( "%s", __FUNCTION__ );
    return 1;
}


int LedTask( struct Task *task ) {
    if ( !task->init_enable )
        return -1;
    if ( !task->enable )
        return -1;

    SetLedColor( 0xff, 0x00 );
    delay( 500 );
    SetLedColor( 0x00, 0xff );
    delay( 500 );
    SetLedColor( 0xff, 0x45 );
    delay( 500 );
    SetLedColor( 0xff, 0xff );
    delay( 500 );
    SetLedColor( 0x7c, 0xfc );
    delay( 500 );
    SetLedColor( 0x00, 0x00 );
    delay( 500 );

    return 1;
}
