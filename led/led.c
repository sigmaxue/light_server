#include "wiringPi.h"
#include "stdio.h"
#include "softPwm.h"
#include "led.h"

const int kRedLedPin = 0;
const int kGreenLedPin = 1;
int bLedOpen = 0;

int init = 0;


void *led_loop(void *vargp)   /* thread routine */
{

        LedInit();
        while(1) {
                delay(500);
                if(!bLedOpen) continue;
                RunLedLoop();
        }
}

void InitRedLed() {
	softPwmCreate(kRedLedPin, 0, 100);
	softPwmCreate(kGreenLedPin, 0, 100);
}

void SetLedColor(char r, char g) {
	softPwmWrite(kRedLedPin, r);
	softPwmWrite(kGreenLedPin, g);
}

void LedInit() {
	if(wiringPiSetup() == -1) {
		printf("init error");
		return 1;
	}
	init = 1;
	InitRedLed();

	printf("init ok");
	return 1;
}


int RunLedLoop() {
	if (!init) return -1;

	SetLedColor(0xff, 0x00);
	delay(500);
	SetLedColor(0x00, 0xff);
	delay(500);
	SetLedColor(0xff, 0x45);
	delay(500);
	SetLedColor(0xff, 0xff);
	delay(500);
	SetLedColor(0x7c, 0xfc);
	delay(500);
	SetLedColor(0x00, 0x00);
	delay(500);
	return 1;
}
