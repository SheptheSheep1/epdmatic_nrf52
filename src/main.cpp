#include <Arduino.h>
#include <SPI.h>

// pins
#define EPD_RST_PIN 22

//const uint8_t 

void _HWReset(void){
	digitalWrite(EPD_RST_PIN, HIGH);
	delay(10);
	digitalWrite(EPD_RST_PIN, LOW); // reset display
	delay(10);
	digitalWrite(EPD_RST_PIN, HIGH);
	delay(10);
}

void setup(){
	delay(10); // wait 10ms after power on
	pinMode(EPD_RST_PIN, OUTPUT);
	SPI.begin();
}
void loop(){}
