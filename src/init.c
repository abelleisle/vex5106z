#include <main.h>

Gyro gyro;
unsigned int imeCount;

void initializeIO(){
	pinMode(ANALOG_PORT(1),INPUT_ANALOG);	// Power expander status port
	pinMode(1,OUTPUT);						// LED
	pinMode(2,INPUT);
	pinMode(3,INPUT);
	pinMode(4,INPUT);
}

void initialize(){

	//	Initialize the LCDs.

	lcdInit(uart1);
	lcdClear(uart1);
	lcdSetBacklight(uart1,1);

	//	Setup sensors.

	imeCount = imeInitializeAll();
	gyro=gyroInit(2,0);

	lcdPrint(uart1,1,"%u IMEs :)",imeCount);
	delay(1000);
	lcdPrint(uart1,1,"ready...");
	delay(1000);

}
