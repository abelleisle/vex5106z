#include "main.h"

Gyro gyro;
unsigned int imeCount=0;

void initializeIO(void){
	pinMode(1,INPUT_ANALOG);
}

void initialize(void){
	lcdInit(uart1);
	lcdClear(uart1);
	gyro=gyroInit(2,0);
	imeCount=imeInitializeAll();
}
