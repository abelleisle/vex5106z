#include <main.h>

Sensor intakeFrontLeft,
	   intakeFrontRight,
	   intakeLiftBase,
	   intakeLiftTop,
	   robotGyro;

Gyro gyro;

bool initializeDone = false;

void initializeIO(void){
	intakeFrontLeft  = initSensor(7,ANALOG);
	intakeFrontRight = initSensor(6,ANALOG);
	intakeLiftBase   = initUltrasonic(1,2);
	intakeLiftTop    = initSensor(8,ANALOG);
	//robotGyro        = initSensor(2,GYRO);
}

void initialize(void){
	lcdInit(uart2);
	lcdClear(uart2);
	lcdSetBacklight(uart2,true);

	gyro = gyroInit(2,0);

	imeInitializeAll();

	lcdPrint(uart2,1,"  5106  ZEPHYR  ");

	initializeDone = true;
	delay(2000);
}
