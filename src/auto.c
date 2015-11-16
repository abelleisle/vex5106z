#include <main.h>

#define TARGET_SPEED 65

static unsigned int time;

void autoDelay(unsigned int ms){
	delay(ms);
	time-=ms;
}

void autonomous(){
	static char speed = 0;

	time = 15000;

	do{

		speed+=10;

		motorSet(CANNON1,speed);
		motorSet(CANNON2,speed);
		motorSet(CANNON3,speed);
		motorSet(CANNON4,speed);

		autoDelay(200);

	}while(speed<TARGET_SPEED);

	autoDelay(800);

	motorSet(LIFT1 ,127);
	motorSet(LIFT2 ,127);
	motorSet(INTAKE,127);

	//while(1);
	delay(time);

	motorStopAll();

}
