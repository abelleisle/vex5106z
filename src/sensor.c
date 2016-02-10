#include <main.h>

/**
 * Nowhere else to put this...?
 */

unsigned char MOTOR_USE_MAP[10] = {
	0,0,0,0,0,0,0,0,0,0
};

int getIMEPort(unsigned int port){
	switch(port){
	case CANNON_LEFT  : return CANNON_LEFT_IME;
	case CANNON_RIGHT : return CANNON_RIGHT_IME;
	case LIFT_PUSHER  :
	case INTAKE_1     :
	case INTAKE_2     : return -1;
	case DRIVE_RIGHT  : return DRIVE_RIGHT_IME;
	case DRIVE_LEFT   : return DRIVE_LEFT_IME;
	case LIFT_1       : return LIFT_1_IME;
	case LIFT_2       : return LIFT_2_IME;
	case LIFT_ROTATER : return LIFT_ROTATER_IME;
	}
	return -1;
}

int getIME(unsigned int port){
	int ret;
	imeGet(port,&ret);
	return ret;
}

int getIMEVelocity(unsigned int port){
	int ret;
	imeGetVelocity(port,&ret);
	return ret;
}

/**
 * Actual sensor stuffs
 */

Sensor initSensor(uint32_t port,unsigned char type){
	Sensor s;
	s.type = type;
	switch(s.type){
	case DIGITAL:
		pinMode(port,INPUT);
		s.port = port;
		//s.initial = digitalRead(port);
		break;
	case ANALOG:
		pinMode(port + 13,INPUT_ANALOG);
		s.port = port;
		//s.initial = analogRead(port);
		break;
	case GYRO:
		s.port = 0;
		s.data.gyro = gyroInit(port,0);
		//s.initial = gyroGet(s.data.gyro);
		break;
	case ULTRASONIC:
		s.port = 0;
		s.data.sonic = ultrasonicInit((uint16_t)port,port>>16);
		//s.initial = ultrasonicGet(s.data.sonic);
		break;
	}
	s.value = 0;
	return s;
}

int readSensor(Sensor *s){
	switch(s->type){
	case DIGITAL:
		return (s->value = digitalRead(s->port));
		break;
	case ANALOG:
		return (s->value = analogRead(s->port));
		break;
	case GYRO:
		return (s->value = gyroGet(s->data.gyro));
		break;
	case ULTRASONIC:
		return (s->value = ultrasonicGet(s->data.sonic));
		break;
	}
	return (s->value = -1);
}
