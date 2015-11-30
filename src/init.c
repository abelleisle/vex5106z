#include <main.h>

void initializeIO(){
}

void initialize(){

	zLCDInit();
	zGyroInit();
	zIMEInit();

	delay(1000);

}
