#include <5106z.h>
#include <string.h>

typedef struct {
	 int8_t *speedP;
	 int8_t  speed;
	uint8_t  thresh;
	uint8_t  polarity	:1;
	uint8_t  enable		:1;
	uint8_t  useptr		:1;
	uint8_t  imeable	:3;
} __attribute__ ((packed)) motor_t;

static motor_t motor[11];

void motor_init(uint8_t count,...){
	va_list m;
	uint8_t i;
	memset(motor,0,11*sizeof(motor_t));
	va_start(m,count);
	for(i=0;i<count;i++){
		motor[va_arg(m,int)].enable=true;
	}
	va_end(m);
}

void motor_initIMEs(uint8_t count,...){
	va_list m;
	uint8_t i;
	va_start(m,count);
	for(i=0;i<count;i++){
		motor[va_arg(m,int)].imeable=i;
	}
	va_end(m);
}

int motor_imeReset(uint8_t port){
	if(!motor[port].enable) return -1;
	if(!motor[port].imeable)return -2;
	imeReset(motor[port].imeable);
	return 0;
}

int motor_imeRead(uint8_t port){
	int imeValue;
	if(!motor[port].enable) return -1;
	if(!motor[port].imeable)return -2;
	if(!imeGet(motor[port].imeable,&imeValue))return -3;
	return imeValue;
}

void motor_enable(uint8_t port){
	motor[port].enable=true;
}

void motor_disable(uint8_t port){
	motor[port].enable=false;
}

int motor_togglePolarity(uint8_t port){
	if(!motor[port].enable)return -1;
	return (motor[port].polarity^=true);
}

int motor_setSpeed(uint8_t port,int8_t speed){
	if(!motor[port].enable)return -1;
	return (motor[port].speed=abs(speed)>motor[port].thresh?speed:0);
}

int motor_setSpeedSum(uint8_t port,uint8_t args,...){
	va_list s;
	uint8_t i;
	int8_t speed=0;
	if(!motor[port].enable)return -1;
	va_start(s,args);
	for(i=0;i<args;i++){
		speed+=va_arg(s,int);
	}
	va_end(s);
	return motor_setSpeed(port,speed);
}

int motor_setSpeedPointer(uint8_t port,int8_t *sp){
	if(!motor[port].enable)return -1;
	motor[port].useptr=true;
	motor[port].speedP=sp;
	return 0;
}

int motor_setThreshold(uint8_t port,uint8_t threshold){
	if(!motor[port].enable)return -1;
	return (motor[port].thresh=threshold);
}

int motor_applySpeeds(void){
	uint8_t i;
	for(i=0;i<10;i++){
		if(motor[i].enable){
			if(motor[i].useptr)
				motor[i].speed=*motor[i].speedP;
			motorSet(i,motor[i].speed*(motor[i].polarity?-1:1));
		}
	}
	return 0;
}
