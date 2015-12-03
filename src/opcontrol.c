#include <main.h>
#include <math.h>

const char *song  = "John Cena:d=4,o=5,b=125:4p,4g5,8a5,8f5,8p,1g5,4p,4a#5,8a5,8f5,8p,1g5";
const char *xpst  = "WinXP Login:d=4,o=5,b=160:4d#6.,4a#5.,2g#5.,4d#6,4a#5";
const char *xpst2 = "WinXP Shutdown:d=4,o=5,b=125:4g#6,4d#6,4g#5,2a#5";
const char *bound = "Nobody to Love:d=4,o=5,b=125:4d#5,2g5.,4g5,4f5,2g5,4d#5,4f5,2g5,4d#5,4f5,4g5,4d#5,4d#5,4f5,4g5,4a#4,1c5,4d#5,4f5,2g5,\
4a#5,8g5,8f5,4d#5";
static unsigned char cyc = 0;

void lcdUpdateFunc(void *);

void operatorControl(){

	static char lift,cann;

	zLCDStart();
	zLCDSetUpdateFunc(lcdUpdateFunc);

	while(1){

		/*
		 *	Handle drive controls and update drive motor speeds.
		*/

		zDriveUpdate();

		//	Set the rotating motor speed.

		zMotorSet("Rotater",-zJoyAnalog(2,1)/4);

		// Set the intake's speed.

		zMotorSet("Intake",zGetDigitalMotorSpeed(1,6,JOY_UP,JOY_DOWN,127));

		// Set the lift's speed.

		lift=zGetDigitalMotorSpeed(2,6,JOY_UP,JOY_DOWN,127);

		zMotorSet("Lift 1",lift);
		zMotorSet("Lift 2",lift);

		cann=zJoyAnalog(2,3);//zGetDigitalMotorSpeed(1,5,JOY_UP,JOY_DOWN,127);

		zMotorSet("Left cannon" ,-cann);
		zMotorSet("Right cannon", cann);

		zMotorSet("Misc",zGetDigitalMotorSpeed(2,7,JOY_UP,JOY_DOWN,127));

		if(zJoyDigital(1,7,JOY_LEFT)){
			speakerInit();
			switch(cyc){
			case 0:speakerPlayRtttl(song );break;
			case 1:speakerPlayRtttl(xpst );break;
			case 2:speakerPlayRtttl(xpst2);break;
			case 3:speakerPlayRtttl(bound);break;
			}
			if(++cyc == 4) cyc = 0;
			speakerShutdown();
		}

		delay(10);	// Short delay to allow task switching
	}
}

void lcdUpdateFunc(void *unused_param){
	static double l,r,dist,heading;
	static double cl,cr,ca;

	/*
	 *	Positioning code.
	*/

	l =  zMotorIMEGet("Left drive")  / 627.2L;
	r = -zMotorIMEGet("Right drive") / 627.2L;

	dist=(l - r) * 8.64L;
	heading = fmod(round(dist / 15),360.0L);

	zLCDWrite(1,"%.3lf %.3lf",heading,dist);
	//zLCDWrite(2,"%.3lf %.3lf",l,r);

	/*
	 *	RPM control code.
	*/

	cl = -zMotorIMEGetVelocity("Left cannon")  / 16.3333125L * 9;
	cr =  zMotorIMEGetVelocity("Right cannon") / 16.3333125L * 9;
	ca = (cl + cr) / 2;
	zLCDWrite(2,"RPM: %.3lf",ca);
}
