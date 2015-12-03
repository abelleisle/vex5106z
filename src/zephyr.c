#include <zephyr.h>
#include <main.h>

#include <string.h>

#ifdef LCD_PORT

static void (*_lcdUpdateFunc)(void *);
static char lcdBuffer[2][16];

void zLCDHandler(void *unused_param){
	while(1){
		lcdSetText(LCD_PORT,1,lcdBuffer[0]);
		lcdSetText(LCD_PORT,2,lcdBuffer[1]);

		if(_lcdUpdateFunc)
			_lcdUpdateFunc(unused_param);

		delay(LCD_RATE);
	}
}

void zLCDInit(void){
	lcdInit(LCD_PORT);
	lcdClear(LCD_PORT);
	lcdSetBacklight(LCD_PORT,true);
}

void zLCDStart(void){
	taskCreate(zLCDHandler,
			   TASK_DEFAULT_STACK_SIZE,
			   NULL,
			   TASK_PRIORITY_DEFAULT
			   );

	memset(&lcdBuffer,0,32);
	strcpy(lcdBuffer[0]," libZEPHYR v1.0 ");
}

void zLCDWrite(unsigned char line,const char *text,...){
	va_list args;
	char buf[16];
	va_start(args,text);
	sprintf(buf,text,args);
	va_end(args);
	strcpy(lcdBuffer[line-1],buf);
}

void zLCDSetUpdateFunc(void (*func)(void *)){
	_lcdUpdateFunc = func;
}

void zLCDClearUpdateFunc(void){
	_lcdUpdateFunc = 0;
}

#endif // LCD_PORT

#ifdef GYRO_PORT

static Gyro gyro;
static bool gyroEnabled = false;

void zGyroInit(void){
	if((gyro=gyroInit(2,0))){
		gyroEnabled = true;
	}
}

#endif // GYRO_PORT

/*
 *	A map of what's plugged into the motor ports.
 *	Expected declarations:
 *
 *	DRIVEL	-	Left drive port
 *	DRIVER	-	Right drive port
 *
*/

#define MOTOR_PORT_COUNT	10

#ifdef IME_ENABLE
#define MOTOR_IME_COUNT		7
#endif // IME_ENABLE

const char *MOTOR_PORT_MAP[MOTOR_PORT_COUNT] = {
	"Left cannon",
	"Right cannon",
	"Misc",
	"Port 4",
	"Intake",
	"Right drive",
	"Left drive",
	"Lift 1",
	"Lift 2",
	"Rotater"
};

#ifdef IME_ENABLE

const char *MOTOR_IME_MAP[MOTOR_IME_COUNT] = {
	"Right drive",
	"Left drive",
	"Rotater",
	"Lift 1",
	"Lift 2",
	"Left cannon",
	"Right cannon"
};

static unsigned int imeCount = 0;

void zIMEInit(void){
	imeCount = imeInitializeAll();
}

#endif // IME_ENABLE

void zMotorSet(const char *motor,char speed){
	for(unsigned char i=0;i<MOTOR_PORT_COUNT;i++){
		if(!strcmp(MOTOR_PORT_MAP[i],motor)){
			motorSet(i+1,speed);
			return;
		}
	}
}

char zMotorGet(const char *motor){
	for(unsigned char i=0;i<MOTOR_PORT_COUNT;i++){
		if(!strcmp(MOTOR_PORT_MAP[i],motor)){
			return motorGet(i+1);
		}
	}
	return 0;
}

#ifdef IME_ENABLE

int zMotorIMEGet(const char *motor){
	int IMEValue = 0;
	for(unsigned char i=0;i<imeCount;i++){
		if(!strcmp(MOTOR_IME_MAP[i],motor)){
			imeGet(i,&IMEValue);
			break;
		}
	}
	return IMEValue;
}

int zMotorIMEGetVelocity(const char *motor){
	int IMEValue = 0;
	for(unsigned char i=0;i<imeCount;i++){
		if(!strcmp(MOTOR_IME_MAP[i],motor)){
			imeGetVelocity(i,&IMEValue);
			break;
		}
	}
	return IMEValue;
}

#endif // IME_ENABLE

void zDriveUpdate(void){

#ifdef DRIVE_NORMAL
	char s = joystickGetAnalog(DRIVE_JOY,DRIVE_NORMAL);

	APPLY_THRESH(s,DRIVE_THRESHOLD);

	motorSet(DRIVEL,s);
	motorSet(DRIVER,s);

#else

	char l,r;

	l = joystickGetAnalog(DRIVE_JOY,DRIVE_TANK_LEFT);
	r = joystickGetAnalog(DRIVE_JOY,DRIVE_TANK_RIGHT);

	APPLY_THRESH(l,DRIVE_THRESHOLD);
	APPLY_THRESH(r,DRIVE_THRESHOLD);

	zMotorSet("Left drive",l);
	zMotorSet("Right drive",r);

#endif // DRIVE_NORMAL

}

char zGetDigitalMotorSpeed(unsigned char joy,unsigned char btn_group,unsigned char btn_pos,unsigned char btn_neg,char speed){
	return joystickGetDigital(joy,btn_group,btn_pos) ?  speed :
		   joystickGetDigital(joy,btn_group,btn_neg) ? -speed : 0;
}
