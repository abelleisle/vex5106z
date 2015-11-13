#ifndef MAIN_H_
#define MAIN_H_

#include <API.h>

#define LCD_PORT uart1

#define ANALOG_PORT(x) (x+13)

#ifdef __cplusplus
extern "C" {
#endif

/*
 *	Aliases for all the motors, stored in an enum for convenience.
*/

enum MOTOR_PORT_MAP {
	UNUSED = 0,
	CANNON1,
	CANNON2,
	CANNON3,
	CANNON4,
	INTAKE,
	DRIVER,
	DRIVEL,
	LIFT1,
	LIFT2,
	ROTATER,
};

enum IME_PORT_MAP {
	IDRIVER,
	IDRIVEL,
	IROTATER,
	ILIFT1,
	ILIFT2,
	ICANNON3
};


void autonomous();
void initializeIO();
void initialize();
void operatorControl();

#ifdef __cplusplus
}
#endif

#endif // MAIN_H_
