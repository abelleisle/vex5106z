#include <main.h>
#include <stdint.h>
#include <math.h>

#define PI 3.1415926535L

extern Gyro gyro;	// Originally defined in init.c

static double Vl,	// left wheel velocity
			  Vr,	// right wheel velocity
			  Vc,	// center/average velocity
			  Va,	// angular velocity
			  Vra;	// velocity ratio

static double R;	// radius of current circular path

static double Px=0,	// X position
			  Py=0;	// Y position

void operatorLCD(void *start){
	while(1){

		lcdPrint(uart1,1,"%.3lf, %.3lf",R,Va);

		delay(100);
	}
}

static unsigned int START;

void operatorControl(){

	static char uiinc = 0;	// See below
	static char in=0,lift;	// Used to set the multiple cannon motors to the same joy-stick reading.

	static char joy;

	/*
	 *	Start other tasks.
	*/

	START = ((!digitalRead(3)) | (!digitalRead(4)<<1)) + 1;
	taskCreate(operatorLCD ,TASK_DEFAULT_STACK_SIZE,&START,TASK_PRIORITY_DEFAULT);

	while(1){

		//	Set the drive motors speeds.

		joy=joystickGetAnalog(1,3);
		if(joy < 10 && joy > -10)joy=0;
		motorSet(DRIVEL,joy);
		joy=joystickGetAnalog(1,2);
		if(joy < 10 && joy > -10)joy=0;
		motorSet(DRIVER,joy);

		//	Set the rotating motor speed.

		motorSet(ROTATER,-joystickGetAnalog(2,1)/4);

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

			// Run autonomous (for testing wo/ competition switch).

			if(joystickGetDigital(1,7,JOY_RIGHT))
				autonomous();

			// Test

			if(joystickGetDigital(1,7,JOY_LEFT)){

			}

		}

		delay(10);	// Short delay to allow task switching

		/*
		 *	Get the velocity of the two (IME'd) drive motors.
		*/

		static int vl,vr;

		imeGetVelocity(1,&vl);
		imeGetVelocity(0,&vr);

		/*
		 *	Convert the raw reading to rotations a minute, then inches per minute, then inches
		 *	per second.
		*/

		Vl =  vl /* RPM Divisor         */ / 39.2L
				 /* Wheel Circumference */ * 8
				 /* Minutes to seconds  */ / 60;

		Vr = -vr / 39.2L * 8 / 60;	// Right wheel IME is inverted

		Vc = (Vl + Vr) / 2;			// Average/Center velocity

		/*
		 *	Round down the results to the nearest inch, and enforce a 2 in/s threshold.
		*/

		Vl = floor(Vl);
		Vr = floor(Vr);

		if(Vl < 2 && Vl > -2) Vl = 0;
		if(Vr < 2 && Vr > -2) Vr = 0;

		/*
		 *	Calculate the ratio of the higher velocity to the lowest, for determining the
		 *	radius of the circle the robot is forming.
		*/

		if(((!Vr) & (Vl!=0)) ||
		   ((!Vl) & (Vr!=0)) ){

			/*
			 *	One wheel isn't moving, the radius is the distance between the wheels.
			*/

			R = 15;
			goto CONT;

		}else if((Vr > Vl) & (Vl > 0) ||	// Curve to forwards right
				 (Vl > Vr) & (Vl < 0) ){	// Curve to backwards right

			Vra = Vr / Vl;

		}else if((Vl > Vr) & (Vr > 0) ||	// Curve to forwards left
				 (Vr > Vl) & (Vr < 0) ){	// Curve to backwards left

			Vra = Vl / Vr;

		}else Vra = 0;						// No movement?

		if(Vra<0) Vra *= -1;				// get absolute value of the ratio

		/*
		 *	"Plug-n'-chug" for a radius, assuming that the radius will increase by 7.5 (the
		 *	halfway between the wheels on the robot) when multiplied by the ratio.
		*/

		for(R = 0; R < 200; R++){

			if(R * Vra > R + 7 &&		// Allow a one inch margin of error
			   R * Vra < R + 8)break;	//
		}

CONT:

		/*
		 *	Calculate the anglular velocity of the robot.
		*/

		Va = Vc / R;

		/*
		 *	Determine the increase in X and Y position based on the angular velocity and the
		 *	average total movement.
		*/

		Px += cos(Va) * Vc * .01;	// Convert per second velocity to per 10ms, as the motors
		Py += sin(Va) * Vc * .01;	// have only been at this velocity for that time (above).

	}
}
