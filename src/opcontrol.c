#include <main.h>
#include <stdint.h>
#include <string.h>

enum MOTOR_PORT_MAP {
	UNUSED = 0,
	INTAKE1,
	DRIVEL ,
	DRIVER ,
	LIFT1  ,
	LIFT2  ,
	UNUSED6,
	INTAKE2,
	INTAKE3,
	INTAKE4,
	ROTATER,
};

extern Gyro gyro;

static   signed int ctty=0;
const  unsigned int mtty=3;

void operatorLCD(void *unused){
	while(1){
		switch(ctty){
		default:
		case 0:
			lcdSetText(uart1,1,"   JOHN  CENA   ");
			lcdSetText(uart1,2,"  ============  ");
			break;
		case 1:
			lcdPrint(uart1,1,"MAIN: %0.3f",(float)(powerLevelMain()/1000.0f));
			lcdPrint(uart1,2,"EXP : %0.3f",(float)(analogRead(1)   / 210.0f));
			break;
		case 2:
			lcdPrint(uart1,1,"Gyro: %u",gyroGet(gyro));
		}
		delay(2000);
	}
}

void operatorControl(){

#define getJoy(n) joy[n-1]

	static int8_t joy[8],
				  lift   = 0,
				  rotate = 0,
				  intake = 0,
				  uiinc = 0;

	// Start the LCD task
	taskCreate(operatorLCD,TASK_DEFAULT_STACK_SIZE,NULL,TASK_PRIORITY_DEFAULT);

	while(1){

		joy[0]=joystickGetAnalog(1,1);
		joy[1]=joystickGetAnalog(1,2);
		joy[2]=joystickGetAnalog(1,3);
		joy[3]=joystickGetAnalog(1,4);
		joy[4]=joystickGetAnalog(2,1);
		joy[5]=joystickGetAnalog(2,2);
		joy[6]=joystickGetAnalog(2,3);
		joy[7]=joystickGetAnalog(2,4);

		intake = getJoy(7);
		rotate =-getJoy(5);
		lift   = joystickGetDigital(2,6,JOY_UP  ) ?  127 :
				 joystickGetDigital(2,6,JOY_DOWN) ? -127 :
				 0;

		motorSet(INTAKE1,intake);
		motorSet(INTAKE2,intake);
		motorSet(INTAKE3,intake);
		motorSet(INTAKE4,intake);
		motorSet(ROTATER,rotate);
		motorSet(LIFT1,lift);
		motorSet(LIFT2,lift);

		motorSet(DRIVEL,-getJoy(2));
		motorSet(DRIVER, getJoy(3));

		/*motorSet(PULLER,joystickGetDigital(2,6,JOY_UP  ) ?  127 :
						joystickGetDigital(2,6,JOY_DOWN) ? -127 :
						0);*/

		if(++uiinc==20){
			uiinc=0;
			if(joystickGetDigital(1,7,JOY_UP) ||
			   joystickGetDigital(2,7,JOY_UP) ){
				if(++ctty==mtty)ctty=0;
			}
	   else if(joystickGetDigital(1,7,JOY_DOWN) ||
			   joystickGetDigital(2,7,JOY_DOWN) ){
				if(--ctty==-1)ctty=mtty-1;
			}
		}

		delay(10);
	}
}
