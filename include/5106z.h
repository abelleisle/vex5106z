#ifndef _5106Z_H_
#define _5106Z_H_

#include <API.h>
#include <stdint.h>
#include <stdarg.h>

/*
 *	motor_init - Initialize motors
 *
 *	`count` - The number of motors to enable
 *	`...`	- The port numbers of the motors to enable
 *
 *	Enables motors on the ports specified in the `...` arguments after
 *	`count`, which should say how many ports are currently being
 *	initialized. Motors not initialized now will be unusable until they
 *	are enabled.
 *
*/
void motor_init(uint8_t count,...);
/*
 *	motor_initIMEs
 *
 *	Sets up IMEs for the motors that have them. This function expects the
 *	motors passed to it to be in order of how they are plugged in to the
 *	cortex. There is _no_ error checking, so... yeah.
 *
*/
void motor_initIMEs(uint8_t count,...);
/*
 *	motor_imeReset
 *
 *	Resets the IME on the motor at port `port` if the motor was init'd
 *	using motor_initIMEs() and is enabled.
 *
*/
int  motor_imeReset(uint8_t port);
/*
 *	motor_imeRead
 *
 *	Returns the IME reading of the port `port` if it was init'd using
 *	motor_initIMEs() and is enabled.
 *
*/
int  motor_imeRead(uint8_t port);
/*
 *	motor_enable & motor_disable
 *
 *	`port`	- The port of the motor to enable/disable
 *
 *	These calls modify the `enable` bit of the motor. If a motor is disabled
 *	its speed will be set to 0 and rendered unchangeable until the motor is
 *	re-enabled. Any motor_* calls made to a disabled motor will immediately
 *	return -1.
 *
*/
void motor_enable(uint8_t port);
void motor_disable(uint8_t port);
/*
 *	motor_togglePolarity
 *
 *	`port`	- The port to toggle polarity on
 *
 *	Flips the polarity of the motor on port `port` if its enabled. When a
 *	motor's polarity is enabled the speed written to it is inverted as it
 *	is passed to motorSet(). For example, if a motor's speed is 127 and it's
 *	polarity is enabled, the speed passed to motorSet() is -127.
 *
*/
int motor_togglePolarity(uint8_t port);
/*
 *	motor_setSpeed
 *
 *	`port`	- The port to set the speed of
 *	`speed`	- The speed to set the motor to
 *
 *	Sets the motor to the given speed, if it is enabled. Keep in mind that
 *	the change is not applied immediately, to apply the speeds you must
 *	call motor_applySpeeds().
 *
*/
int motor_setSpeed(uint8_t port,int8_t speed);
/*
 *	motor_setSpeedSum
 *
 *	`port`	- The port to set the speed of
 *	`args`	- The number of arguments following this one
 *	`...`	- `args` variables to use for setting the speed
 *
 *	Functions the same as motor_setSpeed(), but sets the speed to the sum
 *	of the arguments passed after `args`.
 *
*/
int motor_setSpeedSum(uint8_t port,uint8_t args,...);
/*
 *	TODO
 *
*/
int motor_setSpeedPointer(uint8_t port,int8_t *sp);
/*
 *	motor_setThreshold
 *
 *	`port`		- The port to set the threshold of
 *	`threshold`	- The threshold to give the motor
 *
 *	Sets a threshold for the motor's speed. This means that when calls to
 *	set this motor's speed are made in the future, the speed will only be
 *	applied if it's greater than `threshold` or less than -`threshold`.
 *	Otherwise, the speed is set to 0.
 *
*/
int motor_setThreshold(uint8_t port,uint8_t threshold);
/*
 *	motor_applySpeeds
 *
 *	Applys the speeds of the enabled motors to the actual motors.
 *
*/
int motor_applySpeeds(void);

#endif // _5106Z_H_
