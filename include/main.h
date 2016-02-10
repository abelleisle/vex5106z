#ifndef MAIN_H_
#define MAIN_H_

#include <API.h>
#include <stdint.h>

typedef struct{
	int x;
	int y;
}vec2;

typedef enum {
	UP,
	DOWN,
	KEY_UP
} Button;

typedef struct {
	unsigned int num;
	struct Side {
		struct Group {
			Button l;
			Button r;
			Button u;
			Button d;
		} front, back;
		vec2 stick;
	} left, right;
} Controller;

typedef struct {
	enum type {
		DIGITAL,
		ANALOG,
		GYRO,
		ULTRASONIC
	} type;
	union data {
		Gyro gyro;
		Ultrasonic sonic;
	} data;
	unsigned int port;
	int value;
	int initial;
} Sensor;

typedef struct {
	bool        kill_req;
	bool		exiting;
	TaskCode    code;
	TaskHandle  handle;
	void       *param;
} Process;

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_TRPM trpm = 1850;
#define EXTRA_TRPM	 trpm = 1900;

#define PI 3.14159265L

#define LCD_PORT uart2

/**
 * Be sure that getIMEPort() matches these values (see sensor.c).
 */

enum MOTOR_MAP {
	CANNON_LEFT = 1,
	CANNON_RIGHT,
	LIFT_PUSHER,
	INTAKE_1,
	INTAKE_2,
	DRIVE_RIGHT,
	DRIVE_LEFT,
	LIFT_1,
	LIFT_2,
	LIFT_ROTATER
};

enum MOTOR_IME_MAP {
	DRIVE_RIGHT_IME = 0,
	DRIVE_LEFT_IME,
	LIFT_ROTATER_IME,
	LIFT_1_IME,
	LIFT_2_IME,
	CANNON_LEFT_IME,
	CANNON_RIGHT_IME
};

extern unsigned char MOTOR_USE_MAP[10];

#define motorTake(p,k)   MOTOR_USE_MAP[p-1] = k;
#define motorFree(p)     MOTOR_USE_MAP[p-1] = 0;

#define motorSetK(p,s,k) if(!MOTOR_USE_MAP[p-1] || MOTOR_USE_MAP[p-1] == k){ motorSet(p,s); }
#define motorSetN(p,s)   motorSetK(p,s,0)

#define motorSetBK(p,s,k,b) motorSetK(p,b.u ? s : b.d ? -s : 0,k)
#define motorSetBN(p,s,b)   motorSetN(p,b.u ? s : b.d ? -s : 0)

#define motorCopyK(p1,p2,k) motorSetK(p1,motorGet(p2),k)
#define motorCopyN(p1,p2)   motorSetN(p1,motorGet(p2))

int getIMEPort(unsigned int port);
int getIME(unsigned int port);
int getIMEVelocity(unsigned int port);

/**
 * Controller library functions
 */

#define keyUp(b)   (b == KEY_UP)
#define keyDown(b) (b == DOWN)

#define onKeyUp(b)   if(keyUp(b))
#define onKeyDown(b) if(keyDown(b))

void setEvent(Controller *c);

/**
 * Sensor library functions
 */

#define LIGHT_THRESH_DEFAULT 50
#define SONIC_THRESH_DEFAULT 5

#define initUltrasonic(p1,p2) initSensor((p2<<16)|p1,ULTRASONIC)
Sensor initSensor(uint32_t port,unsigned char type);

#define getSensor(s) (s.value)
int readSensor(Sensor *s);

#define diffSensor(s) 		(s.value - s.initial)
#define underSensor(s,t)	(diffSensor(s) < -t)
#define overSensor(s,t)		(diffSensor(s) > t)

/**
 * Process library functions
 */

#define taskInit(h,p) h = h ? h : taskCreate(h##Code,TASK_DEFAULT_STACK_SIZE,p,TASK_PRIORITY_DEFAULT)

/**
 * Main function declarations
 */

extern void softwareReset(void);

void autonomous();
void initializeIO();
void initialize();
void operatorControl();

#ifdef __cplusplus
}
#endif

#endif // MAIN_H_
