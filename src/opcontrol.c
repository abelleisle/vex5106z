#include <main.h>
#include <stdint.h>
#include <math.h>

extern Gyro gyro;	// Originally defined in init.c

/*
 *	These keep track of the current LCD 'window' and the number of 'windows' that exist.
 */

static   signed int ctty=0;
const  unsigned int mtty=4;

static double imeDriveLeft;
static double imeDriveRight;
static double imeRotater;

static double gyroRotation;

/*
 *	This runs as a separate task to update the LCD.
 */

void operatorLCD(void *unused){
	static int ime;

	while(1){

		/*
		 *	Display information according to the current 'window'.
		 */

		switch(ctty){
		default:

		case 0:	//	Welcome screen

			lcdSetText(uart1,1,"   JOHN  CENA   ");
			lcdPrint(uart1,2,"  ===========%3d",motorGet(CANNON1));
			break;

		case 1:	//	Battery levels in volts

			lcdPrint(uart1,1,"MAIN: %0.3f",(float)(powerLevelMain()/1000.0f));
			lcdPrint(uart1,2,"EXP : %0.3f",(float)(analogRead(1)   / 280.0f));
			break;

		case 2:	//	Gyroscope readings

			imeGet(IROTATER,&ime);

			double rot = ime / 3.0L / 627.2L * 10;

			rot = floor(rot) / 10;

			lcdPrint(uart1,1,"Gyro: %d",gyroGet(gyro));
			lcdPrint(uart1,2,"Rot.: %.1lf",rot);

			break;

		case 3:

			lcdPrint(uart1,1,"%.3lf",imeDriveLeft);
			lcdPrint(uart1,2,"%.3lf",imeDriveRight);

			break;
		}

		delay(500);
	}
}

void operatorPoll(){
	static int idl,idr,ir,gr;
	while(1){

		/*
		 *	Read in rotations of motors and the current angle of the robot.
		 */

		imeGet(DRIVEL,&idl);			// Get reading
		imeDriveLeft  = idl / 627.0L;	// Convert to wheel rotations
		imeDriveLeft *=  8.64L;			// Convert to distance in inches

		imeGet(DRIVER,&idr);			// Get reading
		imeDriveRight  = idr / 627.0L;	// Convert to wheel rotations
		imeDriveRight *=  8.64L;		// Convert to distance in inches

		imeGet(ROTATER,&ir);
		imeRotater = ir / 627.0L;

		gyroRotation = gyroGet(gyro);

		delay(100);
	}
}

void operatorControl(){

	static bool selfAim = false;

	static char uiinc = 0;	// See below

	static char in,lift;	// Used to set the multiple cannon motors to the same joy-stick reading.

	taskCreate(operatorLCD ,TASK_DEFAULT_STACK_SIZE,NULL,TASK_PRIORITY_DEFAULT);
	taskCreate(operatorPoll,TASK_DEFAULT_STACK_SIZE,NULL,TASK_PRIORITY_DEFAULT);

	while(1){

		//	Set the drive motors speeds.

		motorSet(DRIVEL,joystickGetAnalog(1,3));
		motorSet(DRIVER,joystickGetAnalog(1,2));

		//	Set the rotating motor speed.

		if(!selfAim){

			motorSet(ROTATER,-joystickGetAnalog(2,1)/2);

		}else{

			//static int gc=0,gl=0;

			//gl = gc;
			//gc = gyroGet(gyro);

		}

		//	Set the cannon's speed.

		in=joystickGetAnalog(2,3);

		motorSet(CANNON1,in);
		motorSet(CANNON2,in);
		motorSet(CANNON3,in);
		motorSet(CANNON4,in);

		// Set the intake's speed.

		motorSet(INTAKE,joystickGetDigital(1,6,JOY_UP  )?  127 :
				joystickGetDigital(1,6,JOY_DOWN)? -127 :
						0 );

		// Set the lift's speed.

		lift=joystickGetDigital(2,6,JOY_UP  )?  127 :
				joystickGetDigital(2,6,JOY_DOWN)? -127 :
						0 ;

		motorSet(LIFT1,lift);
		motorSet(LIFT2,lift);

		/*
		 *	Miscellaneous operation handlers.
		 */

		if(++uiinc==20){	// Equates to every 200ms
			uiinc=0;

			// Goto next 'window'.

			if(joystickGetDigital(1,7,JOY_UP) ||
					joystickGetDigital(2,7,JOY_UP) ){
				if(++ctty==mtty)ctty=0;
			}

			// Goto previous 'window'.

			if(joystickGetDigital(1,7,JOY_DOWN) ||
					joystickGetDigital(2,7,JOY_DOWN) ){
				if(--ctty==-1)ctty=mtty-1;
			}

			// Run autonomous (for testing wo/ competition switch).

			if(joystickGetDigital(1,7,JOY_RIGHT))
				autonomous();

			// Goto test area.

			if(joystickGetDigital(1,7,JOY_LEFT)){
				static double target = 0;

				if(!target) target = (imeDriveLeft+imeDriveRight) / 2 + 13;

				motorSet(DRIVEL,60);
				motorSet(DRIVER,60);

				while((imeDriveLeft+imeDriveRight) / 2 < target)
					delay(10);

				motorSet(DRIVEL,0);
				motorSet(DRIVER,0);

			}

		}

		delay(10);	// Short delay to allow task switching.
	}
}
//Totally Theoretical PseudoCode for AutoShoot
//Based on IME data:
double lVel;//=velocity of left side
double rVel;//=velocity of right side
double cVel;//=velocity of center (calculated below)
double aVel;//=angular velocity (calculated below)

//Used to find rectangular vectors * double aVel=angular velocity of robot
//Used to store the rectangular vectors:
double xVel1;
double xVel2;
double yVel1;
double yVel2;

//Final Position Vectors
double xPos;
double yPos;

//Vector Assignments:
void Vectors(){
while(1){//something in the brackets
	int i;
	i++;
	cVel=(lVel+rVel)/2;
	if(lVel>rVel){
		aVel=cVel/(16*lVel/(rVel-lVel));
	}
	if(rVel>lVel){
		aVel=cVel/(16*rVel/(lVel-rVel));
	}
	else{
		aVel=0;
				if(i%2==0){
					xVel1=cos(aVel)*cVel;
					yVel1=sin(aVel)*cVel;
				}
				else{
					xVel2=cos(aVel)*cVel;
					yVel2=sin(aVel)*cVel;
				}

		xPos+=((xVel1+xVel2)/2);//*time elapsed between cycles
				yPos+=((yVel1+yVel2)/2);//*time elapsed between cycles
				delay(20);
	}
}
}

