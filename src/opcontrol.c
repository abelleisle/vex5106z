#include <main.h>

void lcdUpdateFunc(void *);

void operatorControl(){

	static char lift;

	zLCDStart();
	zLCDSetUpdateFunc(lcdUpdateFunc);

	while(1){

		/*
		 *	Handle drive controls and update drive motor speeds.
		*/

		zDriveUpdate();

		//	Set the rotating motor speed.

		zMotorSet("ROTATER",-zJoyAnalog(2,1)/4);

		// Set the intake's speed.

		zMotorSet("INTAKE",zGetDigitalMotorSpeed(1,6,JOY_UP,JOY_DOWN,127));

		// Set the lift's speed.

		lift=zGetDigitalMotorSpeed(2,6,JOY_UP,JOY_DOWN,127);

		zMotorSet("LIFT1",lift);
		zMotorSet("LIFT2",lift);

		delay(10);	// Short delay to allow task switching
	}
}

void lcdUpdateFunc(void *unused_param){
	static double liftIME;

	liftIME = (zMotorIMEGetVelocity("LIFT1") - zMotorIMEGetVelocity("LIFT2")) / 2 / 16.3333125L;

	zLCDWrite(1,"%.3lf",liftIME);
}
