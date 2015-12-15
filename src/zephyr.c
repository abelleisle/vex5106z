#include <zephyr.h>
#include <main.h>

#define PI =3.14159

#include <string.h>

#ifdef LCD_PORT
#endif // LCD_PORT

#ifdef GYRO_PORT

static Gyro gyro;
static bool gyroEnabled = false;

void zGyroInit(void){
	if((gyro=gyroInit(2,0))){
		gyroEnabled = true;
	}
}

int zGyroGet(void){
	return gyroGet(gyro);
}

void zGyroReset(void){
	gyroReset(gyro);
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
	"Intake",
	"Intake",
	"Right drive",
	"Left drive",
	"Lift 1",
	"Lift 2",
	"Rotater"
};

static unsigned int mInUse[10]={
	0,0,0,0,0,0,0,0,0,0
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

void zMotorSet(const char *motor,int speed,unsigned int id){
	for(unsigned char i=0;i<MOTOR_PORT_COUNT;i++){
		if(!strcmp(MOTOR_PORT_MAP[i],motor) && mInUse[i] == id){
			if(speed >  127)speed = 127;
			if(speed < -127)speed = -127;
			motorSet(i+1,speed);
			//return;
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

void zMotorTake(const char *motor,unsigned int id){
	for(unsigned char i=0;i<MOTOR_PORT_COUNT;i++){
		if(!strcmp(MOTOR_PORT_MAP[i],motor)){
			mInUse[i] = id;
		}
	}
}
void zMotorReturn(const char *motor){
	for(unsigned char i=0;i<MOTOR_PORT_COUNT;i++){
		if(!strcmp(MOTOR_PORT_MAP[i],motor)){
			mInUse[i] = 0;
		}
	}
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

bool zMotorIMEReset(const char *motor){
	for(unsigned char i=0;i<imeCount;i++){
		if(!strcmp(MOTOR_IME_MAP[i],motor)){
			return imeReset(i);
		}
	}
	return false;
}

#endif // IME_ENABLE

void zDriveUpdate(void){

#ifdef DRIVE_NORMAL
	char y = joystickGetAnalog(DRIVE_JOY,DRIVE_NORMAL);
	char x = joystickGetAnalog(DRIVE_JOY,4);

	APPLY_THRESH(x,DRIVE_THRESHOLD);
	APPLY_THRESH(y,DRIVE_THRESHOLD);

	zMotorSet("Left drive" ,y+x,0);
	zMotorSet("Right drive",y-x,0);

#else

	char l,r;

	l = joystickGetAnalog(DRIVE_JOY,DRIVE_TANK_LEFT);
	r = joystickGetAnalog(DRIVE_JOY,DRIVE_TANK_RIGHT);

	APPLY_THRESH(l,DRIVE_THRESHOLD);
	APPLY_THRESH(r,DRIVE_THRESHOLD);

	zMotorSet("Left drive",l,0);
	zMotorSet("Right drive",r,0);

#endif // DRIVE_NORMAL

}

char zGetDigitalMotorSpeed(unsigned char joy,unsigned char btn_group,unsigned char btn_pos,unsigned char btn_neg,char speed){
	return joystickGetDigital(joy,btn_group,btn_pos) ?  speed :
		   joystickGetDigital(joy,btn_group,btn_neg) ? -speed : 0;
}
