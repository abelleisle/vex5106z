#include <main.h>

unsigned int imeCount;

void initializeIO(){
	pinMode(ANALOG_PORT(1),INPUT_ANALOG);
}

void initialize(){
	imeCount = imeInitializeAll();
}
