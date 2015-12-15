#include "main.h"

#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define PI =3.14159
/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 *
*/

/* notes for Clyne:
 * Other processes should not interupt the collection of data:
 * rotation calculation should be and data collection should be a high priority/primary task
 * Main task= manages tasks, init variables, reset functions (sensors, etc)
 * Primary = sensor data collections and calculations
 * Secondary = drive tasks
 *
 *
 *
 */
static long lVel,rVel,deltaPos,deltaTime,startTheta,
					  heading,lDist,rDist,xPos,yPos,
					  track=15;
void operatorControl() {
	heading=startTheta;
	while (1) {

		//get time 1 the first time
		heading=((lDist-rDist)/(track*2*PI))*360+startTheta;//heading in degrees. might need to be radians for cosine functions
		//get time 2
		deltaPos=(lVel+rVel)/2*deltaTime;
		xPos+=cos(heading)*deltaPos;
		yPos+=sin(heading)*deltaPos;
		//get time 1
		delay(20);

	}
}
