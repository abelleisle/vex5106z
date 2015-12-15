#include <main.h>
#include <stdint.h>
#include <math.h>

#define GOAL_DISTANCE 120

/*
 * RTTTL songs for speaker usage.
 */

const char *song  = "John Cena:d=4,o=5,b=125:4p,4g5,8a5,8f5,8p,1g5,4p,4a#5,8a5,8f5,8p,1g5";
const char *xpst  = "WinXP Login:d=4,o=5,b=160:4d#6.,4a#5.,2g#5.,4d#6,4a#5";
const char *xpst2 = "WinXP Shutdown:d=4,o=5,b=125:4g#6,4d#6,4g#5,2a#5";
const char *bound = "Nobody to Love:d=4,o=5,b=125:4d#5,2g5,4p,4g5,4f5,2g5,4d#5,4f5,2g5,4d#5,4f5,4g5,4d#5,4d#5,4f5,4g5,4a#4,1c5,4d#5,4f5,2g5,\
4a#5,8g5,8f5,4d#5";

/*
 * Contains how many milliseconds it took to enter operatorControl().
 */

static unsigned long opmillis = 0;

/*
 * Contains a light sensor value calculated at init time for object detection.
 */

static int lightThresh = 0;

static char cann = 0;
static double rpm  = 0,trpm = 1750, arpm = 0;

static double xpos=0,ypos=0;

static bool cannonProcRun = false,
			armProcRun = false,
			aimProcRun = false;

TaskHandle taskLCD,
		   taskCannon,
		   taskArm,
		   taskMove,
		   taskAim;

void lcdUpdateFunc(void *);
void cannonProc(void *);
void armProc(void *);
void moveProc(void *);
void aimProc(void *);

/**
 * Cause the Cortex to reset, which results in the Cortex restarting the
 * operator control code.
 *
 * This reset is accomplished through setting two bits, SYSRESETREQ and
 * VECTRESET, in the Application Interrupt and Reset Control Register (AIRCR),
 * which is at a known location in memory. SYSRESETREQ will actually request
 * the system reset, while VECTRESET is 'reserved for debugging'. This second
 * bit may not need to be set; it's only set here because others have found it
 * necessary.
 */

#define AIRCR_ADDR		0xE000ED0C
#define VECTKEY			0x05FA
#define SYSRESETREQ		(1<<2)
#define VECTRESET		(1<<0)

void softwareReset(void){

	/*
	 * Read the current value of the AIRCR, since some flags currently set in
	 * the register need to remain the same in order for the reset to work.
	 */

	uint32_t AIRCR = *((uint32_t *)AIRCR_ADDR);

	/*
	 * Here we save the lower 16 bits of the register, write a special key to
	 * the higher 16 bits that'll allow the write to occur, and then set the
	 * reset flags.
	 */

	AIRCR = (AIRCR & 0xFFFF) | (VECT_KEY << 16) | SYSRESETREQ | VECTRESET;

	// Update the AIRCR.

	*((uint32_t *)0xE000ED0C) = AIRCR;

	/*
	 * This instruction causes the program to wait until the previous memory
	 * write is complete, ensuring it is taken into effect and the reset
	 * request is made.
	 */

	asm("DSB");

	// Wait until the system reset completes.

	while(1);
}

/******************************************************************************
 * OPERATOR CONTROL                                                           *
 ******************************************************************************/

void operatorControl(){
	static unsigned char ui_inc = 0;
	static unsigned char cyc = 0;
	static char lift;

	opmillis = millis();
	lightThresh = analogRead(8) - 60;

	taskLCD=taskCreate(lcdUpdateFunc,TASK_DEFAULT_STACK_SIZE,NULL,TASK_PRIORITY_DEFAULT);
	taskMove=taskCreate(moveProc,TASK_DEFAULT_STACK_SIZE,NULL,TASK_PRIORITY_DEFAULT+1);

	while(1){

		/*
		 *	Handle drive controls and update drive motor speeds.
		 */

		zDriveUpdate();

		//	Set the rotating motor speed.

		if(!aimProcRun)
			zMotorSet("Rotater",-zJoyAnalog(1,1) / 4,0);

		// Set the intake's speed.

		zMotorSet("Intake",
				  zGetDigitalMotorSpeed(1,5,JOY_UP,JOY_DOWN,127),
				  0
				  );

		// Set the lift's speed.

		if(!armProcRun){
			lift = zGetDigitalMotorSpeed(1,6,JOY_UP,JOY_DOWN,127);
			zMotorSet("Lift 1",lift,0);
			zMotorSet("Lift 2",lift,0);
		}

		// Set the cannon's speed.

		if(!cannonProcRun){
			zMotorSet("Left cannon" ,-cann,0);
			zMotorSet("Right cannon", cann,0);
		}

		if(++ui_inc == 50){
			ui_inc = 0;

			if(zJoyDigital(1,5,JOY_UP) && zJoyDigital(1,5,JOY_DOWN))
				cpu_reset();

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
			}else if(zJoyDigital(1,7,JOY_RIGHT)){
				zGyroReset();
				zMotorIMEReset("Rotater");
			}else if(zJoyDigital(1,7,JOY_UP))
				taskAim = taskCreate(aimProc,TASK_DEFAULT_STACK_SIZE,NULL,TASK_PRIORITY_DEFAULT);
			else if(zJoyDigital(1,7,JOY_DOWN))
				aimProcRun = false;

			if(zJoyDigital(1,8,JOY_UP)){
				if(cannonProcRun) arpm += 50;
				else if(cann < 120)cann += 10;
			}else if(zJoyDigital(1,8,JOY_DOWN)){
				if(cannonProcRun) arpm -= 50;
				else if(cann > -120)cann -= 10;
			}else if(zJoyDigital(1,8,JOY_LEFT)){
				if(!cannonProcRun)
					taskCannon = taskCreate(cannonProc,TASK_DEFAULT_STACK_SIZE,NULL,TASK_PRIORITY_DEFAULT);
				else
					cannonProcRun = false;
			}

			if(zJoyDigital(1,8,JOY_RIGHT))
				taskArm = taskCreate(armProc,TASK_DEFAULT_STACK_SIZE,NULL,TASK_PRIORITY_DEFAULT);
		}

		delay(10);	// Short delay to allow task switching
	}
}

/**
 * The Position-Tracker process.
 *
 * This process tries to track the position of the robot, using a combination
 * of motor encoders and the gyroscope. This process is created directly, and
 * is expected to run at a higher priority than other tasks.
 */

void moveProc(void *unused_param){
	static double l,r,lv,rv;
	static double dist,head;
	static double dpos;
	while(1){

		l =  zMotorIMEGet("Left drive")  / 627.2L;
		r = -zMotorIMEGet("Right drive") / 627.2L;

		// random # -> motor RPM -> wheel RPM (inches per minute) -> inches per millisecond

		lv =  zMotorIMEGetVelocity("Left drive")  / 39.2L * 8.64L / 60000;
		rv = -zMotorIMEGetVelocity("Right drive") / 39.2L * 8.64L / 60000;

		// total distance traveled since init
		dist = (l - r) * 8.64L;

		head = fmod(dist / 15,360.0L) / 2 * 90;
		head = /*(head +*/ zGyroGet()/*) / 2*/;

		dpos = (lv+rv) / 2 * 20;
		dpos *= 1000;
		dpos = floor(dpos) / 1000;

		ypos += sin(head * PI / 180) * dpos;
		xpos += cos(head * PI / 180) * dpos;

		//lcdPrint(uart1,1,"%.3lf,%.3lf",xpos,ypos);

		delay(20);
	}
}

/**
 * The Auto-Aim process.
 *
 * This function will attempt to keep the cannon aimed at a constant angle,
 * adjusting its position when the robot's orientation changes by using the
 * gyroscope and the encoder build into the rotater motor.
 */

void aimProc(void *procPtr){
	static double cangle,	// Current angle (of rotater)
				  rangle,	// 'Robot' angle (target angle
				  angle;	// Angle between x-axis and line from robot to goal
	static double dist;		// Distance of robot from goal
	double xpos2,goal2;

	aimProcRun = true;

	/*
	 * Claim necessary motors.
	 */

	zMotorTake("Rotater",1);

	/*
	 * Run until a stop is requested.
	 */

	while(aimProcRun){

		/*
		 * Read orientation sensors.
		 */

		cangle = (int)floor(zMotorIMEGet("Rotater") / 627.2L * 112.5);
		rangle = zGyroGet();

		xpos2 = pow(xpos,2);
		goal2 = pow(GOAL_DISTANCE - ypos,2);
		dist = sqrt(xpos2 + goal2);
		angle = acos((xpos2 + pow(dist,2) - goal2) / (2 * xpos * dist));
		rangle = (angle - PI / 2) * 180 / PI;

		lcdPrint(uart1,2,"%lf",angle);

		/*
		 * Adjust aim if necessary.
		 */

		if(cangle > rangle + 3)
			zMotorSet("Rotater",30,1);
		else if(cangle < rangle - 3)
			zMotorSet("Rotater",-30,1);
		else
			zMotorSet("Rotater",0,1);

		delay(100);
	}

	/*
	 * Free motors.
	 */

	zMotorReturn("Rotater");
}

/**
 * The RPM-Targeter process.
 *
 * This will attempt to keep the cannon motors running at a constant speed,
 * determined through a target RPM set by the user.
 */

void cannonProc(void *procPtr){
	static double cl,cr,ca;
	static int speed;

	cannonProcRun = true;

	/*
	 *	Initialize variables.
	 *	These need to be static so that their values are preserved through
	 *	task switchs but will retain their values when the function is
	 *	re-called, so here we reset them.
	 */

	speed = 20;
	rpm = cl = cr = ca = 0;

	/*
	 * Reserve needed motors.
	 */

	zMotorTake("Left cannon",2);
	zMotorTake("Right cannon",2);

	/*
	 *	Here we increase the power provided to the motors until our target
	 *	RPM is reached.
	 */

	do{

		/*
		 *	Bring up the speed, --exiting if we go too high.-- (crossed out)
		 *	The only reasonable explanation for an error such as the speed
		 *	getting too high is a hardware fault in the robot (bad motor,
		 *	bad IME, ...).
		 */

		speed += 5;
		if(speed > 120)
			speed = 127;

		/*
		 *	Set the power levels, and allow the motors to adjust.
		 */

		zMotorSet("Left cannon" ,-speed,2);
		zMotorSet("Right cannon", speed,2);

		delay(400);

		/*
		 *	Calculate the average RPM, and continue if our target is met.
		 */

		cl =  zMotorIMEGetVelocity("Left cannon")  / 16.3333125L * 9;
		cr = -zMotorIMEGetVelocity("Right cannon") / 16.3333125L * 9;
		ca = /*(cl +*/ cr/*) / 2*/;
		rpm = ca;

		lcdPrint(uart1,2,"%.3lf/%.3lf",cl,cr);

	}while(cannonProcRun && ca < trpm);

	if(!cannonProcRun)
		return;

	/*
	 *	Once we reach our target, we 'idle' at the sped and adjust as
	 *	necessary. `cannInUse` will be cleared by any user attempt to use the
	 *	motor.
	 */

	while(cannonProcRun){

		/*
		 *	Update RPM values.
		 */

		cl =  zMotorIMEGetVelocity("Left cannon")  / 16.3333125L * 9;
		cr = -zMotorIMEGetVelocity("Right cannon") / 16.3333125L * 9;
		ca = /*(cl +*/ cr/*) / 2*/;
		rpm = ca;

		/*
		 *	Guess an RPM.
		 */

		if(xpos < 20)
			trpm = 1850;
		else if(xpos < 40)
			trpm = 1750;
		else if(xpos < 60)
			trpm = 1650;
		else
			trpm = 1550;

		trpm += arpm;

		/*
		 *	Handle fluxuations in RPM by adjusting the power level if the
		 *	motor RPMs go out of range after three 'tries' (over a course
		 *	of 600ms).
		 */

		if(ca < trpm - 40){
			speed++;
			zMotorSet("Left cannon" ,-speed,2);
			zMotorSet("Right cannon", speed,2);
		}else if(ca > trpm + 50){
			speed--;
			zMotorSet("Left cannon" ,-speed,2);
			zMotorSet("Right cannon", speed,2);
		}

		lcdPrint(uart1,2,"%4.0lf/%4.0lf",trpm,rpm);

		delay(200);
	}

	zMotorSet("Left cannon" ,0,2);
	zMotorSet("Right cannon",0,2);

	zMotorReturn("Left cannon");
	zMotorReturn("Right cannon");
}

/**
 * The Ball-Pusher process.
 *
 * This process will handle the pusher that actually sends the ball into the
 * cannon through either an instant push or a detected one, in which the lift
 * is ran until a ball is seen via a light sensor.
 */

void armProc(void *procPtr){
	static unsigned int start;

	armProcRun = true;

	/*
	 * Claim necessary motors.
	 */

	zMotorTake("Lift 1",3);
	zMotorTake("Lift 2",3);
	zMotorTake("Misc",3);

	/*
	 * Collect the time we started this operation, but negate it so we can add
	 * the finish millis() call to collect a difference in milliseconds.
	 */

	start = -millis();

	/*
	 * Just run the pusher if it was requested by the user.
	 */

	if(zJoyDigital(1,7,JOY_DOWN))
		goto PUSH;

	/*
	 * Run the lift and wait until a ball is detected by the light sensor. Once
	 * a ball is seen, stop the lift so that the pusher can do its job.
	 */

	zMotorSet("Lift 1",127,3);
	zMotorSet("Lift 2",127,3);

	while(armProcRun && analogRead(8) > lightThresh)
		delay(10);

	if(!armProcRun)
		return;

	delay(300);

	zMotorSet("Lift 1",0,3);
	zMotorSet("Lift 2",0,3);

	/*
	 * Push a ball into the cannon.
	 */
PUSH:

	zMotorSet("Misc",127,3);
	delay(1000);
	zMotorSet("Misc",-127,3);
	delay(1000);
	zMotorSet("Misc",0,3);

	/*
	 * 'Stop' the timer and print the result. Do this on the first line in case
	 * the RPM tracker or another process is using the second.
	 */

	start += millis();
	lcdPrint(uart1,1,"%ums",start);

	/*
	 * Release the used motors.
	 */

	zMotorReturn("Lift 1");
	zMotorReturn("Lift 2");
	zMotorReturn("Misc");

	armProcRun = false;
}

/**
 * The LCD update function, registered at init time to be called by libZephyr's
 * LCD code.
 */

void lcdUpdateFunc(void *unused_param){
	unsigned long elapsed;
	while(1){
		/*
		 * Track elapsed time since operatorControl() entrance.
		 */

		elapsed = millis() - opmillis;
		lcdPrint(uart1,1,"%02d:%02d",(int)(elapsed / 60000),(int)((elapsed / 1000) % 60));

		delay(LCD_RATE);
	}
}
