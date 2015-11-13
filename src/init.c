#include <main.h>

Gyro gyro;
unsigned int imeCount;

void initializeIO(){
	pinMode(ANALOG_PORT(1),INPUT_ANALOG);	// Power expander status port
	pinMode(1,OUTPUT);						// LED
}

void initialize(){

	//	Initialize the LCDs.

	lcdInit(uart1);
	lcdClear(uart1);
	lcdSetBacklight(uart1,1);

	lcdInit(uart2);
	lcdClear(uart2);
	lcdSetBacklight(uart2,1);

	//	Setup sensors.

	imeCount = imeInitializeAll();
	gyro=gyroInit(2,0);

	lcdPrint(uart1,1,"%u IMEs :)",imeCount);

	lcdPrint(uart2,1,"ERR: Dark theme ");
	lcdPrint(uart2,2,"     not found! ");

	delay(2000);

	lcdPrint(uart1,1,"ready...");
	lcdPrint(uart2,1,"...yeah.        ");
	lcdPrint(uart2,2,"                ");

	delay(1000);

}
