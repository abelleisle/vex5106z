#include <main.h>

#define TARGET_RPM 1970

static unsigned int time;

void autoDelay(unsigned int ms){
	delay(ms);
	time-=ms;
}

void autonomous(){
	static int    trpm;
	static double rpm;
	static char cs;

	time = 15000;
	cs = rpm = trpm = 0;

	do{

		cs+=cs<100?10:0;

		motorSet(CANNON1,cs);
		motorSet(CANNON2,cs);
		motorSet(CANNON3,cs);
		motorSet(CANNON4,cs);

		autoDelay(800);

		imeGetVelocity(ICANNON3,&trpm);
		rpm=abs(trpm)/24.5L;
		rpm*=25.0L;

	}while(rpm<TARGET_RPM);

	motorSet(LIFT1,127);
	motorSet(LIFT2,127);

	delay(time);

	// Stop										4000ms remaining

	motorStopAll();

}
