#include <main.h>

void initializeIO(){
}

void initialize(){

	pinMode(20,INPUT_ANALOG);
	pinMode(13,INPUT_ANALOG);

	lcdInit(LCD_PORT);
	lcdClear(LCD_PORT);
	lcdSetBacklight(LCD_PORT,true);

	zGyroInit();
	zIMEInit();

	delay(1000);

}
