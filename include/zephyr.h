#ifndef ZEPHYR_H_
#define ZEPHYR_H_

#include <main.h>

#define APPLY_THRESH(n,t)	if(n < t && n > -t){ n = 0;}

/*
 *	Comment to disable LCD support.
*/

#define LCD_PORT	uart1

#define LCD_RATE	500

#ifdef LCD_PORT

void zLCDInit(void);
void zLCDStart(void);

void zLCDWrite(unsigned char,	// 1-based Line Number
			   const char *,	// Text
			   ...);

void zLCDSetUpdateFunc(void (*)(void *));	// Function Pointer
void zLCDClearUpdateFunc(void);

#endif // LCD_PORT

/*
 *	Comment to disable gyro support.
*/

#define GYRO_PORT	2

#ifdef GYRO_PORT

void zGyroInit(void);

#endif // GYRO_PORT

/*
 *	Comment to disable IME support.
*/

#define IME_ENABLE

#ifdef IME_ENABLE

void zIMEInit(void);

#endif // IME_ENABLE

/*
 *	DRIVE_NORMAL will override tank drive options.
*/

#define DRIVE_JOY			1
#define DRIVE_THRESHOLD		10

//#define DRIVE_NORMAL		3

#define DRIVE_TANK_LEFT		3
#define DRIVE_TANK_RIGHT	2

#define zJoyDigital(j,g,b)	joystickGetDigital(j,g,b)
#define zJoyAnalog(j,a)		joystickGetAnalog(j,a)

void zMotorSet(const char *,	// Motor Name
			   char				// Desired Speed
			   );
char zMotorGet(const char *);	// Motor Name

#ifdef IME_ENABLE

int zMotorIMEGet(const char *);			// Motor Name
int zMotorIMEGetVelocity(const char *);	// Motor Name

#endif // IME_ENABLE

void zDriveUpdate(void);

char zGetDigitalMotorSpeed(unsigned char,	// Joystick No.
				   	   	   unsigned char,	// Button Group
						   unsigned char,	// Positive Button
						   unsigned char,	// Negative Button
						   char				// Desired Speed
				   	   	   );

#endif // ZEPHYR_H_
