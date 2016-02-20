#ifndef MAIN_H_
#define MAIN_H_

#include <API.h>
#include <stdint.h>

/**
 * A two-dimensional vector structure.
 *
 * This structure contains a 2D coordinate for organized storage.
 */

typedef struct{
	int x; /**< The 'x' coordinate */
	int y; /**< The 'y' coordinate */
} vec2;

/**
 * An enum for storing a button's position.
 *
 * The controller subsystem allows buttons to generate three signals. These are
 * the expected pressed and unpressed states, named DOWN and UP respectively,
 * along with a third signal called KEY_UP that is sent out upon the immediate
 * release of a button. This allows for one-time events to be called on button
 * releases.
 */

typedef enum {
	UP,		/**< The unpressed state		*/
	DOWN,	/**< The pressed state			*/
	KEY_UP	/**< The 'just-released' state	*/
} Button;

/**
 * A structure for storing all input states of a single controller.
 *
 * This object groups a joystick's axes and buttons in a convenient manner,
 * with simple function calls to poll inputs and update them.
 */

typedef struct {
	unsigned int num;	/**< The joystick's number as seen by PROS */

	/**
	 * The 'Side' structures separate the left and right halves of the
	 * controller.
	 */

	struct Side {

		/**
		 * The 'Group' structure groups the buttons as written on the
		 * controller.
		 */

		struct Group {
			Button l;	/**< The left button, if it exists	*/
			Button r;	/**< The right button, if it exists	*/
			Button u;	/**< The up button					*/
			Button d;	/**< The down button				*/
		} front, back;

		vec2 stick;	/**< The joystick's position, stored as a 2D coordinate. */

	} left, right;

} Controller;

/**
 * A structure for handling most robot sensors.
 *
 * This structure contains the information necessary to handle most of the
 * sensors provided by VEX, and allows for easier sensor reading and handling.
 */

typedef struct {

	/**
	 * A 'type' enum for recognizing the sensor's type.
	 */

	enum type {
		DIGITAL,	/**< A generic digital sensor	*/
		ANALOG,		/**< A generic analog sensor	*/
		GYRO,		/**< A gyroscore (BROKEN)		*/
		ULTRASONIC	/**< An ultrasonic sensor		*/
	} type;

	/**
	 * A special 'data' item for sensors that need extra variables.
	 */

	union data {
		Gyro gyro;			/**< The gyroscope object provided by PROS	*/
		Ultrasonic sonic;	/**< The ultrasonic object provided by PROS	*/
	} data;

	unsigned int port;	/**< The port that the sensor is connected to	*/
	int value;			/**< The most recent value of the sensor		*/
	int initial;		/**< The initial reading from the sensor		*/
} Sensor;

/**
 * A structure for tracking processes.
 *
 * This object contains most information about a task, as well as flags for
 * controlling the task's state.
 */

typedef struct {
	bool kill_req;		/**< A kill request signal for ending the process	*/
	TaskCode code;		/**< The function that is the task					*/
	TaskHandle handle;	/**< The task handle provided by PROS				*/
	void *param;		/**< The argument provided for the task				*/
} Process;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A macro for setting the target RPM to it's default value.
 */

#define DEFAULT_TRPM trpm = 1650;

/**
 * A macro for setting the target RPM to a little higher than usually
 * necessary.
 */

#define EXTRA_TRPM	 trpm = 1650;

/**
 * The mathematical constant 'pi', for trig/angular calculations.
 */

#define PI 3.14159265L

/**
 * The port that an LCD is expected to be plugged into.
 */

#define LCD_PORT uart2

/**
 * An enum for the set of motor ports on the Cortex, for tagging what motors
 * are.
 *
 * The getIMEPort() function defined in sensor.c takes these values for
 * accessing their IME counterparts, so insure that this definition matches
 * what getIMEPort() expects.
 */

enum MOTOR_MAP {
	CANNON_LEFT = 1,
	CANNON_RIGHT,
	LIFT_PUSHER,
	INTAKE_1,
	INTAKE_2,
	DRIVE_RIGHT,
	DRIVE_LEFT,
	LIFT_1,
	LIFT_2,
	LIFT_ROTATER
};

/**
 * An enum for tagging IMEs.
 *
 * getIMEPort() will allow access to these values by referencing their
 * MOTOR_MAP counterparts, so hard-coding these shouldn't be necessary.
 */

enum MOTOR_IME_MAP {
	DRIVE_RIGHT_IME = 0,
	DRIVE_LEFT_IME,
	LIFT_ROTATER_IME,
	LIFT_1_IME,
	LIFT_2_IME,
	CANNON_LEFT_IME,
	CANNON_RIGHT_IME
};

extern unsigned char MOTOR_USE_MAP[10];

#define motorTake(p,k)   MOTOR_USE_MAP[p-1] = k;
#define motorFree(p)     MOTOR_USE_MAP[p-1] = 0;

#define motorSetK(p,s,k) if(!MOTOR_USE_MAP[p-1] || MOTOR_USE_MAP[p-1] == k){ motorSet(p,s); }
#define motorSetN(p,s)   motorSetK(p,s,0)

#define motorSetBK(p,s,k,b) motorSetK(p,b.u ? s : b.d ? -s : 0,k)
#define motorSetBN(p,s,b)   motorSetN(p,b.u ? s : b.d ? -s : 0)

#define motorCopyK(p1,p2,k) motorSetK(p1,motorGet(p2),k)
#define motorCopyN(p1,p2)   motorSetN(p1,motorGet(p2))

int getIMEPort(unsigned int port);
int getIME(unsigned int port);
int getIMEVelocity(unsigned int port);

/**
 * Controller library functions
 */

#define keyUp(b)   (b == KEY_UP)
#define keyDown(b) (b == DOWN)

#define onKeyUp(b)   if(keyUp(b))
#define onKeyDown(b) if(keyDown(b))

void setEvent(Controller *c);

/**
 * Sensor library functions
 */

#define LIGHT_THRESH_DEFAULT 50
#define SONIC_THRESH_DEFAULT 5

#define initUltrasonic(p1,p2) initSensor((p2<<16)|p1,ULTRASONIC)
Sensor initSensor(uint32_t port,unsigned char type);

#define getSensor(s) (s.value)
int readSensor(Sensor *s);

#define diffSensor(s) 		(s.value - s.initial)
#define underSensor(s,t)	(diffSensor(s) < -t)
#define overSensor(s,t)		(diffSensor(s) > t)

/**
 * Process library functions
 */

#define taskInit(h,p) h = h ? h : taskCreate(h##Code,TASK_DEFAULT_STACK_SIZE,p,TASK_PRIORITY_DEFAULT)

/**
 * Main function declarations
 */

extern void softwareReset(void);

void autonomous();
void initializeIO();
void initialize();
void operatorControl();

#ifdef __cplusplus
}
#endif

#endif // MAIN_H_
