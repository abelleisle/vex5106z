/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Copyright (c) 2011-2014, Purdue University ACM SIG BOTS.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Purdue University ACM SIG BOTS nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PURDUE UNIVERSITY ACM SIG BOTS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Purdue Robotics OS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */
#include "main.h"
#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
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
lDist*r=theta
rDist*(r+15)=theta
cDist*(r+7.5)=theta
theta/lDist=r
rDist*(theta/lDist+15)=theta
rDist*theta/lDist+15*rDist=theta
rDist/lDist+15*rDist/theta=1
1-15*rDist/theta=rDist/lDist
theta=(1-15*rDist)*lDist/rDist
theta=(lDist-15*rDist*lDist)/rDist
theta=lDist/rDist-heading*lDist
theta=
*/
static int 		rRot,lRot,cRot,time1,time2,iHeading;
static float 	lDist,
				theta,
				rDist,
				cDist,
				wheelD=3,
				pi=3.14159,
				heading=15,
				startTheta,
				fHeading;
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
void operatorControl() {
	heading=startTheta;
	while (1) {
		time1=micros();
		cRot=(encoderGet(encoder1)-encoderGet(encoder2));
		cDist=(cRot*pi)/120;
		fHeading=(cDist/12)*(360/(2*pi));
		iHeading=round(fHeading)%360;

		lcdPrint(uart2,1,"iHeading: %d",iHeading);

		if(fHeading<360){
			motorSet(10,-60);
			motorSet(1,20);

		}else{
			motorSet(10,0);
			motorSet(1,0);

		}
		/* lRot=encoderGet(encoder1);
		rRot=encoderGet(encoder2);
		lDist=(lRot*pi)/120;
		rDist=(rRot*pi)/120;
		lcdPrint(uart2,1,"iHeading: %d",iHeading);
		if(digitalRead(1)==0){
		motorSet(1,120);
		}
		else if(digitalRead(2)==0){
		motorSet(10,-120);
		}
		else{
		motorSet(1,0);
		motorSet(10,0);
		}
		*/
		delay(20);

	}
}
