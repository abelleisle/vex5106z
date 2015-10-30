#include <main.h>

Gyro gyro;
unsigned int imeCount;

void initializeIO(){
	pinMode(ANALOG_PORT(1),INPUT_ANALOG);
}

void initialize(){
	imeCount = imeInitializeAll();
	gyro=gyroInit(2,0);
	lcdInit(uart1);
	lcdClear(uart1);
	lcdSetBacklight(uart1,1);
}
