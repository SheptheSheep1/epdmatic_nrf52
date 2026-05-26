#include <Arduino.h>
#include "wiring_constants.h"
#include <SPI.h>
#include <stdint.h>
#define PIN_NC 0

// pins
#define EPD_RST_PIN (32 + 04)
#define EPD_CLK_PIN (0 + 24)
#define EPD_DIN_PIN (0 + 22)
#define EPD_CS_PIN (32 + 00)
#define EPD_DC_PIN (00 + 11)

#define _EPD_spiSettings (SPISettings(4000000, MSBFIRST, SPI_MODE0)) // NRF_SPIM_MODE_0, 4MHz
//const uint8_t 

void _EPD_HWReset(void);
void _EPD_SWReset(void);
void _EPD_initDisplay(void);
void _EPD_writeCommand(uint8_t);
void _EPD_writeData(uint8_t);
void _EPD_writeData(const uint8_t*, uint16_t n);

void setup(void);
void loop(void);

void _EPD_HWReset(void){
	digitalWrite(EPD_RST_PIN, HIGH);
	delay(10);
	digitalWrite(EPD_RST_PIN, LOW); // reset display
	delay(10);
	digitalWrite(EPD_RST_PIN, HIGH);
	delay(10);
}

void _EPD_SWReset(void){
	_EPD_writeCommand(0x12);
}

void _EPD_InitDisplay(void){
	delay(10); // wait 10ms after power on
	pinMode(EPD_RST_PIN, OUTPUT);
	pinMode(EPD_CS_PIN, OUTPUT);
	SPI.setPins(PIN_NC, EPD_CLK_PIN, EPD_DIN_PIN);
	SPI.begin();
	_EPD_HWReset();
	_EPD_SWReset();
	delay(10);
	_EPD_writeCommand(0x01);
}

void _EPD_writeCommand(uint8_t command){
	SPI.beginTransaction(_EPD_spiSettings);
	digitalWrite(EPD_CS_PIN, LOW);
	SPI.transfer(command);
	digitalWrite(EPD_CS_PIN, HIGH);
	SPI.endTransaction();
}

void _EPD_writeData(uint8_t data){
	SPI.beginTransaction(_EPD_spiSettings);
	digitalWrite(EPD_DC_PIN, HIGH);
	SPI.transfer(data);
	digitalWrite(EPD_DC_PIN, LOW);
	SPI.endTransaction();
}

void _EPD_writeData(const uint8_t* data, uint16_t count){

}

void setup(){

}
void loop(){
	Serial.println("Hello world!");
}
