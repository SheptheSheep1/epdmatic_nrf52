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
#define EPD_BUSY_PIN (32 + 06)

#define _EPD_spiSettings (SPISettings(4000000, MSBFIRST, SPI_MODE0)) // NRF_SPIM_MODE_0, 4MHz

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

void _EPD_initDisplay(void){
	delay(10); // wait 10ms after power on
	pinMode(EPD_RST_PIN, OUTPUT);
	pinMode(EPD_CS_PIN, OUTPUT);
	digitalWrite(EPD_CS_PIN, HIGH);
	digitalWrite(EPD_DC_PIN, LOW);
	SPI.setPins(PIN_NC, EPD_CLK_PIN, EPD_DIN_PIN);
	SPI.begin();
	_EPD_HWReset();
	_EPD_SWReset();
	delay(10);
	// display initialization
	
	_EPD_writeCommand(0x01); // gate driver output
	_EPD_writeData(0xF9); // A[8:0] = num_gates - 1; A[8:0] = 250 (num rows) - 1, A[8:0] = 249 = 0x0F9
	_EPD_writeData(0x00); // 0x0F9, A[8] = 0 (base 2)
	_EPD_writeData(0x00);

	_EPD_writeCommand(0x11);
	_EPD_writeData(0x01);

	_EPD_writeCommand(0x44); // set ram x addr
	_EPD_writeData(0x00); // start X ram pos addr (this is also POR/defaults...so this is kinda just for lulz)
	_EPD_writeData(0x0F); // end X ram pos addr
	
	_EPD_writeCommand(0x45); // set ram y addr
	_EPD_writeData(0x00); // start pos (9 bits)
	_EPD_writeData(0x00);
	_EPD_writeData(0xF9); // end pos (9 bits) (249)
	_EPD_writeData(0x00);

	_EPD_writeCommand(0x3C); // border waveform
	_EPD_writeData(0x05);

	// waveform LUT
	_EPD_writeCommand(0x18);
	_EPD_writeData(0x80);
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
	digitalWrite(EPD_CS_PIN, LOW);
	SPI.transfer(data);
	digitalWrite(EPD_DC_PIN, LOW);
	digitalWrite(EPD_CS_PIN, HIGH);
	SPI.endTransaction();
}

void _EPD_writeData(const uint8_t* data, size_t count){
	SPI.beginTransaction(_EPD_spiSettings);
	digitalWrite(EPD_DC_PIN, HIGH);
	for(size_t i = 0; i<count; i++){
		SPI.transfer(*data++);
	}
	digitalWrite(EPD_DC_PIN, LOW);
	SPI.endTransaction();
}

void setup(){
_EPD_initDisplay();
}
void loop(){
	Serial.println("Hello world!");
}
