#include <main.h>

#define TARGET_RPM 1700

void autonomous(){
	/*static double cl,cr,ca;
	static char speed;

	speed = 30;

	do{
		speed += 10;
		zMotorSet("Left cannon",-speed);
		zMotorSet("Right cannon",speed);
		delay(800);
		cl =  zMotorIMEGetVelocity("Left cannon")  / 16.3333125L * 9;
		cr = -zMotorIMEGetVelocity("Right cannon") / 16.3333125L * 9;
		ca = (cl + cr) / 2;
	}while(ca < TARGET_RPM);

	zMotorSet("Misc",127);
	delay(900);
	zMotorSet("Misc",-127);
	delay(900);
	zMotorSet("Misc",0);

	motorStopAll();*/
}
