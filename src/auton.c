#include <main.h>

extern Gyro gyro;

#ifndef PI
#define PI 3.14159265L
#endif // PI

#define RPM_DEFAULT_MAX		160				//	Max RPM of high-speed motor
#define RPM_CANNON_MAX		4000			//	Max RPM of cannon (motor + gears)
#define RPM_CANNON_INC		31.49606299L	//	Expected change in RPM per joystick increment

#define RPM_THRESHOLD		100				//	A margin of error for matching the target RPM

#define INC_FAULT			2				//	Boost given to motors when left and right don't match RPM
#define INC_NORMAL			2				//	Used to match actual velocity to the target

#define ROT_THRESHOLD		(PI/8)			//	Margin of error for auto-aiming
#define ROT_SPEED			30				//	How fast to rotate the cannon for aiming (for linear aiming only)

static double rpmTarget = 0;	// Contains the desired RPM

static double rpmCannL = 0;		// Left side's RPM
static double rpmCannR = 0;		// Right side's RPM
static double rpmCannA = 0;		// Average of the two above

static double rotCannon = 0;	// Cumulative rotation of the cannon in radians

/**
 * Calculates RPMs from cannon motors.
 *
 * There are two IMEs on the cannon, one per each side. This function converts the raw
 * velocity reading from the motors and converts it to rotations per minute (RPM).
 * Results are stored in global variables defined above, as well as an average of the
 * two.
 */

void cannUpdate(void){
	int cl,cr,rc;

	/*
	 *	Read raw velocity values.
	*/

	imeGetVelocity(ICANNONL,&cl);
	imeGetVelocity(ICANNONR,&cr);

	/*
	 *	Divide by a divisor given in API.h and multiply the result by the cannon's gear ratio.
	*/

	rpmCannL = cl / 24.5L * 25.0L;
	rpmCannR = cr / 24.5L * 25.0L;

	/*
	 *	Calculate an average of the two results from above.
	*/

	rpmCannA = (rpmCannL + rpmCannR) / 2;

	/*
	 *	Find the cumulative rotation of the cannon for auto-aiming.
	*/

	imeGet(IROTATER,&rc);

	rotCannon = rc / 627.2L * 5;
	rotCannon *= 2*PI;				// convert to radians

}

/**
 * Sets a target RPM for the cannon.
 *
 * This calculates a target RPM by simply multiplying the current position of a jostick axis
 * by a predetermined increment that is 1/127th of the highest possible RPM of the cannon.
 *
 * @param a value returned from a call to joystickGetAnalog()
 */

void cannTarget(char jpos){
	rpmTarget = jpos * RPM_CANNON_INC;
}

/**
 * Runs/Updates the actual speed of the cannon motors.
 *
 * Speeds for each side of the cannon are kept inside the scope of this function. Two checks
 * are made before applying the motor speeds. The first one will increase the speed of one
 * side of the cannon if the RPMs of the two sides aren't reasonably equal. The second check
 * will bring the overall speeds up or down depending on where the actual average RPM is
 * compared to the target. Following these checks is the setting of the motor speeds.
 */

void cannRun(void){
	static char speedCannL = 0;
	static char speedCannR = 0;

	/*
	 *	Make sure cannon motors are up to about the same speed (as in RPM).
	*/

		 if(rpmCannR < rpmTarget + RPM_THRESHOLD && rpmCannL < rpmCannR - RPM_THRESHOLD) speedCannL += INC_FAULT;
	else if(rpmCannL < rpmTarget + RPM_THRESHOLD &&	rpmCannR < rpmCannL - RPM_THRESHOLD) speedCannR += INC_FAULT;

	/*
	 *	Adjust motor velocities to match the target.
	*/

	if(rpmTarget > rpmCannA + RPM_THRESHOLD){
		speedCannL += INC_NORMAL;
		speedCannR += INC_NORMAL;
	}else if(rpmTarget < rpmCannA - RPM_THRESHOLD){
		speedCannL -= INC_NORMAL;
		speedCannR -= INC_NORMAL;
	}

	/*
	 *	Apply the calculated motor speeds.
	*/

	motorSet(CANNON1,speedCannL);
	motorSet(CANNON2,speedCannR);
	motorSet(CANNON3,speedCannL);
	motorSet(CANNON4,speedCannR);
}

/**
 * Aims the cannon towards a specified location(?)
 *
 * ... . Also sets the motor speed.
 *
 * @param a rotation to base calculations of off (preferably that of the robot's body)
 */

void cannAim(double r){
	static char speed = 0;

	/*
	 *	Do simple linear adjustments to have okay-ish aim. Once self-location is accomplished
	 *	a more accurate function could be used to adjust aim.
	*/

		 if(rotCannon > r + ROT_THRESHOLD) speed =  ROT_SPEED;
	else if(rotCannon < r - ROT_THRESHOLD) speed = -ROT_SPEED;
	else								   speed =  0;

	/*
	 *	Apply the resulting speed to the motor.
	*/

	motorSet(ROTATER,speed);
}
