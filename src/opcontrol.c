#include "main.h"

#define NEAR_THRESH 0.05f

enum MOTOR {
	UNUSED = 0,
	ROTATER,
	DRIVEL,
	DRIVER,
	LIFT1,
	LIFT2,
	INTAKE,
	UNUSED7,
	UNUSED8,
	UNUSED9,
	UNUSED10
};

static unsigned char idx=0;
static float intakeRotation=0;

int nearDegree(float target){
	if(intakeRotation<target-NEAR_THRESH)return  1;
	if(intakeRotation>target+NEAR_THRESH)return -1;
	else				    			 return  0;
}

void operatorControlLCD(void *param){
	static int imeValue=0;
	static float nearTarget=0;
	while(1){
		imeGet(idx,&imeValue);
		intakeRotation=imeValue/1037.0f;
		lcdPrint(uart1,1,"%u %d",imeCount,nearDegree(nearTarget));
		lcdPrint(uart1,2,"%u %d",idx,imeValue);
		if(joystickGetDigital(1,7,JOY_LEFT)){
			if(idx)idx--;
		}else if(joystickGetDigital(1,7,JOY_RIGHT)){
			if(idx<imeCount-1)idx++;
		}
		if(joystickGetDigital(1,8,JOY_LEFT)){
			nearTarget-=.25;
		}else if(joystickGetDigital(1,8,JOY_RIGHT)){
			nearTarget+=.25;
		}
		delay(300);
	}
}

void operatorControl(void){
	static char liftSpeed=0;
	static char intakeSpeed=0;
	taskCreate(operatorControlLCD,TASK_DEFAULT_STACK_SIZE,NULL,TASK_PRIORITY_DEFAULT);
	while(1){
		// Set drive motors
		motorSet(DRIVEL, joystickGetAnalog(1,3));
		motorSet(DRIVER,-joystickGetAnalog(1,2));
		liftSpeed=-(joystickGetDigital(1,6,JOY_UP  )? 127:
				    joystickGetDigital(1,6,JOY_DOWN)?-127:0);
		motorSet(LIFT1,liftSpeed);
		motorSet(LIFT2,liftSpeed);
		intakeSpeed=-(joystickGetDigital(1,5,JOY_UP  )? 127:
					  joystickGetDigital(1,5,JOY_DOWN)?-127:0);
		motorSet(INTAKE,intakeSpeed);
		motorSet(ROTATER,joystickGetAnalog(1,1));

		// test motor
		//motorSet(10,joystickGetDigital(1,7,JOY_UP)?127:0);

		delay(20);
	}
}
