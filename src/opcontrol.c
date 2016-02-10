#include <main.h>
#include <math.h>

#define AUTO_SKILLS

extern Sensor intakeFrontLeft,
			  intakeFrontRight,
			  intakeLiftBase,
			  intakeLiftTop;//,
			  //robotGyro;

extern Gyro gyro;

extern bool initializeDone;

static TaskHandle taskLCD  = NULL,
				  taskPos  = NULL,
				  taskCan  = NULL,
				  taskArm  = NULL,
				  taskLift = NULL,
				  taskAim  = NULL;

static float xpos = 0,
		     ypos = 0;

static double cangle = 0;

static double rpm = 0, trpm = 1850, arpm = 0;
static bool cannReady = false;

static Controller c[2];

static Button No = UP;

void taskLCDCode(void *);
void taskPosCode(void *);
void taskCanCode(void *);
void taskLiftCode(void *);
void taskArmCode(void *);
void taskAimCode(void *);

void operatorControl(void){
	static bool invert = false;

	DEFAULT_TRPM;
	c[0].num = 1;
	c[1].num = 2;

	/**
	 * Insure that the initialization functions were executed.
	 */

	if(!initializeDone){
		initializeIO();
		initialize();
	}

	/**
	 * Get initial readings from each sensor.
	 */

	intakeFrontLeft.initial = readSensor(&intakeFrontLeft);
	intakeFrontRight.initial = readSensor(&intakeFrontRight);
	intakeLiftBase.initial = readSensor(&intakeLiftBase);
	intakeLiftTop.initial = readSensor(&intakeLiftTop);
	//robotGyro.initial = readSensor(&robotGyro);

	taskInit(taskLCD,NULL);
	taskInit(taskPos,NULL);

	if(taskCan)
		taskDelete(taskCan);

	while(1){

		/**
		 * Update sensor values.
		 */

		readSensor(&intakeFrontLeft);
		readSensor(&intakeFrontRight);
		readSensor(&intakeLiftBase);
		readSensor(&intakeLiftTop);
		//readSensor(&robotGyro);

		/**
		 * Read the joystick!!!
		 */

		setEvent(&c[0]);
		setEvent(&c[1]);

		if(keyUp(c[1].left.front.l)){
			taskInit(taskCan,&c[1].left.front.l);
		}else if(keyUp(c[1].left.front.u)){
			/*if(!taskLift){
				c[1].left.front.u = DOWN;
				taskLift = taskCreate(taskLiftCode,TASK_DEFAULT_STACK_SIZE,&c[1].left.front.u,TASK_PRIORITY_DEFAULT);
			}*/
			taskInit(taskAim,&c[1].left.front.u);
		}else if(keyUp(c[1].left.front.d)){
			taskInit(taskArm,&c[1].left.front.d);
		}

		if(keyUp(c[1].right.front.u)){
			arpm += 50;
		}else if(keyUp(c[1].right.front.d)){
			arpm -= 50;
		}else if(keyUp(c[1].right.front.l) | keyDown(c[0].right.front.l)){
			softwareReset();
		}

		if(keyUp(c[0].right.front.r) | keyUp(c[1].right.front.r)){
			invert ^= true;
		}

		motorSetN(DRIVE_LEFT ,c[0].left.stick.y);
		motorSetN(DRIVE_RIGHT,c[0].right.stick.y);

		motorSetN(INTAKE_2,(c[0].left.back.u | c[1].left.back.u) ? 127 : (c[0].left.back.d | c[1].left.back.d) ? -127 : 0);
		motorSetN(INTAKE_1,invert ? -motorGet(INTAKE_2) : motorGet(INTAKE_2));

		motorSetN(LIFT_1,c[1].right.back.u ? 127 : c[1].right.back.d ? -127 : 0);
		motorSetN(LIFT_2,motorGet(LIFT_1));

		motorSetN(LIFT_ROTATER,-c[1].right.stick.x / 4);

		delay(20);
	}
}

static unsigned int ballPos = 0;

void taskLiftCode(void *unused){
	Button *kill = (Button *)unused;

	motorTake(INTAKE_1,1);
	motorTake(INTAKE_2,1);

	do{
		if(!underSensor(intakeLiftTop,LIGHT_THRESH_DEFAULT)){
			if(cangle < 20 && cangle > -20){
				motorTake(LIFT_1,1);
				motorTake(LIFT_2,1);
				motorSetK(LIFT_1,127,1);
				motorSetK(LIFT_2,127,1);
			}else{
				motorSetK(LIFT_1,0,1);
				motorSetK(LIFT_2,0,1);
				motorFree(LIFT_1);
				motorFree(LIFT_2);

				if(!underSensor(intakeFrontLeft,LIGHT_THRESH_DEFAULT)  &&
				   !underSensor(intakeFrontRight,LIGHT_THRESH_DEFAULT) ){
					motorSetK(INTAKE_1,127,1);
					motorSetK(INTAKE_2,127,1);
				}else{
					motorSetK(INTAKE_1,0,1);
					motorSetK(INTAKE_2,0,1);
				}
			}
		}else{
			motorSetK(INTAKE_1,0,1);
			motorSetK(INTAKE_2,0,1);
			motorSetN(LIFT_1,0);
			motorSetN(LIFT_2,0);
		}
	}while(!keyDown(*kill));

	motorFree(INTAKE_1);
	motorFree(INTAKE_2);

	while(keyDown(*kill))
		delay(100);

	taskLift = NULL;
}

void taskAimCode(void *unused){
	Button *kill = (Button *)unused;

	static double target = 0;

	motorTake(LIFT_ROTATER,4);

	target = cangle;

	do{

		if(cangle > target){
			motorSetK(LIFT_ROTATER,30,4);
		}else if(cangle < target){
			motorSetK(LIFT_ROTATER,-30,4);
		}else{
			motorSetK(LIFT_ROTATER,0,4);
		}

		delay(100);
	}while(!keyDown(*kill));

	motorFree(LIFT_ROTATER);

	while(keyDown(*kill))
		delay(100);

	taskAim = NULL;
}

void taskPosCode(void *unused){
	static unsigned int dlime,drime;
	static double /*l,r,*/lv,rv;
	static double /*dist,*/head;
	static double dpos;

	dlime = getIMEPort(DRIVE_LEFT);
	drime = getIMEPort(DRIVE_RIGHT);

	while(1){

		cangle = (int)floor(getIME(getIMEPort(LIFT_ROTATER)) / 627.2L * 112.5);
		cangle += 0.28L * (cangle / 25);

		/**
		 * Get IME values.
		 */

		//l = getIME(dlime) / 627.2L;
		//r = getIME(drime) / 627.2L;

		/**
		 * Get IME velocity values and convert to inches per millisecond:
		 *
		 * random # ->
		 * motor RPM ->
		 * wheel RPM (inches per minute) ->
		 * inches per millisecond
		 */

		lv =  getIMEVelocity(dlime) / 39.2L * 12.566L / 60000;
		rv = -getIMEVelocity(drime) / 39.2L * 12.566L / 60000;

		/**
		 * Get the distance thing.
		 */

		//dist = (l - r) * 8.64L;

		/**
		 * Calculate heading, then trash the result in favor of the gyroscope.
		 */

		//head = fmod(dist / 15,360.0L) / 2 * 90;
		//head = 0;//getSensor(robotGyro);
		head = gyroGet(gyro);

		/**
		 * Calculate the delta position thing.
		 */

		dpos = (lv+rv) / 2 * 20;
		dpos *= 1000;
		dpos = floor(dpos) / 1000;

		// Adjust position values.

		ypos += sin(head * PI / 180) * dpos;
		xpos += cos(head * PI / 180) * dpos;

		delay(20);
	}
}

void taskCanCode(void *unused){
	Button *kill = (Button *)unused;

	static unsigned int clime,crime;
	static double cl,cr,ca;
	static int speed;

	clime = getIMEPort(CANNON_LEFT);
	crime = getIMEPort(CANNON_RIGHT);

	speed = 20;
	rpm = cl = cr = ca = 0;

	motorTake(CANNON_LEFT,2);
	motorTake(CANNON_RIGHT,2);

	do{

		speed += 10;
		if(speed > 120)
			speed = 127;

		motorSetK(CANNON_LEFT,-speed,2);
		motorSetK(CANNON_RIGHT,speed,2);

		delay(300);

		cl =  getIMEVelocity(clime) / 16.3333125L * 9;
		cr = -getIMEVelocity(crime) / 16.3333125L * 9;
		rpm = ca = cr;//(cl + cr) / 2;

	}while(ca < trpm);

	while(!keyDown(*kill)){

		/*
		 *	Update RPM values.
		 */

		cl =  getIMEVelocity(clime) / 16.3333125L * 9;
		cr = -getIMEVelocity(crime) / 16.3333125L * 9;
		rpm = ca = cr;//(cl + cr) / 2;

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
			speed += 2 * ((trpm - rpm) / 60);
			motorSetK(CANNON_LEFT,-speed,2);
			motorSetK(CANNON_RIGHT,speed,2);
			cannReady = false;
		}else if(ca > trpm + 40){
			speed --;
			motorSetK(CANNON_LEFT,-speed,2);
			motorSetK(CANNON_RIGHT,speed,2);
			cannReady = false;
		}else
			cannReady = true;

		delay(100);
	}

	motorSetK(CANNON_LEFT,0,2);
	motorSetK(CANNON_RIGHT,0,2);

	motorFree(CANNON_LEFT);
	motorFree(CANNON_RIGHT);

	while(keyDown(*kill))
		delay(100);

	taskCan = NULL;
}

void taskArmCode(void *unused){
	if(keyDown(c[1].left.front.r))
		goto PUSH;

	while(!underSensor(intakeLiftTop,LIGHT_THRESH_DEFAULT) && ((rpm < trpm - 30) | (rpm > trpm + 50)))
		delay(100);

	/*while(ballPos != 5)
		delay(100);*/

PUSH:

	motorTake(LIFT_PUSHER,3);

	motorSetK(LIFT_PUSHER,127,3);
	delay(500);
	motorSetK(LIFT_PUSHER,-127,3);
	delay(800);
	motorSetK(LIFT_PUSHER,0,3);

	motorFree(LIFT_PUSHER);

	taskArm = NULL;
}


void taskLCDCode(void *unused){
	static unsigned int pos = 1;
	unsigned int lcdInput;
	static double cangle = 0;

	while(1){

		switch(pos){
		case 0:
			lcdPrint(LCD_PORT,1,"(%.2lf,%.2lf)",xpos,ypos);
			lcdPrint(LCD_PORT,2,"%s: %u",taskGetState(taskLift) == TASK_RUNNING ? "Running" : "Stopped",ballPos);
			break;
		case 1:
			lcdPrint(LCD_PORT,1,"%.0lf | %.2lf\n",trpm,rpm);
			lcdPrint(LCD_PORT,2,"                ");
			break;
		case 2:
			lcdPrint(LCD_PORT,1,"%u %u %u %u",
					 	 	 	taskGetState(taskPos),
					 	 	 	taskGetState(taskCan),
								taskGetState(taskArm),
								taskGetState(taskLift));
			break;
		case 3:
			lcdPrint(LCD_PORT,1,"%lf",cangle);
			lcdPrint(LCD_PORT,2,"                ");
			break;
		}

		lcdInput = lcdReadButtons(LCD_PORT);

		if((lcdInput & LCD_BTN_LEFT) && pos)
			pos--;
		else if((lcdInput & LCD_BTN_RIGHT) && pos < 3)
			pos++;
		else if(lcdInput & LCD_BTN_CENTER)
			softwareReset();

		delay(500);
	}
}

#ifdef AUTO_SKILLS

/*****************************************************************************
 * AUTONOMOUS - SKILLS                                                       *
 *****************************************************************************/

void autonomous(){
	static unsigned long inc = 0;

	EXTRA_TRPM;
	intakeLiftTop.initial = readSensor(&intakeLiftTop);

	taskInit(taskPos,&No);
	taskInit(taskAim,&No);
	taskInit(taskCan,&No);

	while(1){

		if(++inc == 50){
			inc = 0;
			lcdPrint(LCD_PORT,1,"%.0lf RPM",rpm);
		}

		//readSensor(&intakeLiftTop);
		//if(rpm >= trpm){// && underSensor(intakeLiftTop,LIGHT_THRESH_DEFAULT)){
		if(rpm > trpm - 30 && rpm < trpm + 50){

			lcdPrint(LCD_PORT,2,"Launch!");

			delay(500);

			motorSet(LIFT_PUSHER,127);
			delay(500);
			motorSet(LIFT_PUSHER,-127);
			delay(800);
			motorSet(LIFT_PUSHER,0);

			lcdPrint(LCD_PORT,2,"       ");
		}

		delay(10);
	}
}

#else

/*****************************************************************************
 * AUTONOMOUS - NORMAL                                                       *
 *****************************************************************************/

void autonomous(){
	static unsigned long start,elapsed = 0;
	static unsigned long inc = 0;

	EXTRA_TRPM;
	start = millis();
	intakeLiftTop.initial = readSensor(&intakeLiftTop);

	taskInit(taskCan,&No);

	motorSet(LIFT_1,127);
	motorSet(LIFT_2,127);
	//motorSet(INTAKE_1,127);
	//motorSet(INTAKE_2,127);

	while(1){
		elapsed = millis() - start;

		if(++inc == 50){
			inc = 0;
			lcdPrint(LCD_PORT,1,"%02d:%02d",(int)(elapsed / 60000),(int)((elapsed / 1000) % 60));
			lcdPrint(LCD_PORT,2,"%.0lf RPM",rpm);
		}

		readSensor(&intakeLiftTop);
		if(underSensor(intakeLiftTop,LIGHT_THRESH_DEFAULT)){


			delay(200);
			motorSet(LIFT_1,0);
			motorSet(LIFT_2,0);
			//motorSet(INTAKE_1,0);
			//motorSet(INTAKE_2,0);

			if(rpm >= trpm){
				motorSet(LIFT_PUSHER,127);
				delay(500);
				motorSet(LIFT_PUSHER,-127);
				delay(500);
				motorSet(LIFT_PUSHER,0);

				motorSet(LIFT_1,127);
				motorSet(LIFT_2,127);
				//motorSet(INTAKE_1,127);
				//motorSet(INTAKE_2,127);
			}
		}

		delay(10);
	}
}

#endif // AUTO_SKILLS
