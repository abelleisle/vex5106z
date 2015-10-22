#include <main.h>
#include <string.h>
#include <5106z.h>

enum MOTOR_PORT_MAP {
	UNUSED = 0,
	ROTATER,
	DRIVEL,
	DRIVER,
	LIFT1,
	LIFT2,
	INTAKE1,
	INTAKE2,
	INTAKE3,
	INTAKE4,
	PULLER,
};

static int8_t joyx   = 0,	// Contains the left joystick's x position
			  joyy   = 0,	// Contains the left joystick's y position
			  lift   = 0,	// Contains the desired speed for the lift
			  rotate = 0;	// Contains the desired speed for the lift's axis

void shell(void *unused){
	char *input=(char *)malloc(4*sizeof(char));
	while(1){
		printf("debug@5106Z > ");
		memset(input,0,4);
		//fgets(input,4,stdin);
		input[0]=fgetc(stdin);
		printf("\n\r");
		switch(input[0]){
		case 'v':
			input[2]=fgetc(stdin);
			printf("\n\r");
			if(input[2]=='m')
				printf("Main voltage: %1.3f V\n\r",powerLevelMain()/1000.0f);
			else if(input[2]=='e')
				printf("Expander voltage: %1.3f V\n\r",analogRead(1)/70.0f);
			break;
		case 't':
			printf("Test\n\r");
			break;
		}
	}
}

void operatorControl(){

	motor_init(10,			// Initialize 6 motor ports
			   ROTATER,
			   DRIVEL,
			   DRIVER,
			   LIFT1,
			   LIFT2,
			   INTAKE1,
			   INTAKE2,
			   INTAKE3,
			   INTAKE4,
			   PULLER);

	motor_togglePolarity(DRIVER );	// Flip the right drive motors
	motor_togglePolarity(ROTATER);	// Flip the lift's rotation motor



	motor_setSpeedPointer(LIFT1  ,&lift  );	// Always set the lift speed with `lift`
	motor_setSpeedPointer(LIFT2  ,&lift  );	//
	motor_setSpeedPointer(ROTATER,&rotate);	// Always set the lift's axis speed
											// with `rotate`

	extern unsigned int imeCount;
	motor_initIMEs(imeCount,
				   DRIVER,
				   DRIVEL,
				   0,
				   0,
				   ROTATER);

	// Launch the shell
	taskCreate(shell,TASK_DEFAULT_STACK_SIZE,NULL,TASK_PRIORITY_DEFAULT);

	while(1){

		digitalWrite(1,lcdReadButtons(LCD_PORT));

		joyx   = joystickGetAnalog(1,4);	// Get joystick positions
		joyy   = joystickGetAnalog(1,3);	//
		lift   = joystickGetAnalog(1,2);	//
		rotate = joystickGetAnalog(1,1);	//

		motor_setSpeedSum(DRIVEL,2,joyy, joyx);	// Set drive speeds
		motor_setSpeedSum(DRIVER,2,joyy,-joyx);	//

		static char huh;
		huh=joystickGetDigital(1,8,JOY_UP)?127:joystickGetDigital(1,8,JOY_DOWN)?-127:0;
		motor_setSpeed(INTAKE1,huh);
		motor_setSpeed(INTAKE2,huh);
		motor_setSpeed(INTAKE3,huh);
		motor_setSpeed(INTAKE4,huh);

		motor_setSpeed(PULLER,joystickGetDigital(1,7,JOY_UP)?127:joystickGetDigital(1,7,JOY_DOWN)?-127:0);

		motor_applySpeeds();	// Apply the motor speeds
	}
}
