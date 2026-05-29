#include <Arduino.h>
#include "variant.h"
#include "wiring_constants.h"
#include <SPI.h>
#include <stdint.h>
#define PIN_NC 0

// pins
#define SLEEPING_DELAY 5000

#define EPD_RST_PIN (32 + 04)
#define EPD_CLK_PIN (0 + 24)
#define EPD_DIN_PIN (0 + 22)
#define EPD_CS_PIN (32 + 00)
#define EPD_DC_PIN (00 + 11)
#define EPD_BUSY_PIN (32 + 06)

#define EPD_WIDTH 122
#define EPD_HEIGHT 250

#define EPD_BUF_SIZE (((EPD_WIDTH + 7) / 8) * EPD_HEIGHT )
#define EPD_BYTES_PER_ROW ((EPD_WIDTH + 7) / 8)

#define _EPD_spiSettings (SPISettings(4000000, MSBFIRST, SPI_MODE0)) // NRF_SPIM_MODE_0, 4MHz

uint8_t bw_buf[EPD_BUF_SIZE];

void _EPD_HWReset(void);
void _EPD_SWReset(void);
void _EPD_initDisplay(void);
void _EPD_writeCommand(uint8_t);
void _EPD_writeData(uint8_t);
void _EPD_writeData(const uint8_t*, size_t);
void _EPD_powerOff(void);
void _EPD_stallBusy(void);

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
	memset(bw_buf, 0xFF, sizeof(bw_buf)); // make it all white
	pinMode(EPD_RST_PIN, OUTPUT);
	pinMode(EPD_CS_PIN, OUTPUT);
	pinMode(EPD_DC_PIN, OUTPUT);
	pinMode(EPD_BUSY_PIN, INPUT);

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
	_EPD_writeData(0x03);

	_EPD_writeCommand(0x44); // set ram x addr
	_EPD_writeData(0x00); // start X ram pos addr (this is also POR/defaults...so this is kinda just for lulz)
	_EPD_writeData((128-1)/8); // end X ram pos addr, real width is 128
	
	_EPD_writeCommand(0x45); // set ram y addr
	_EPD_writeData(0x00); // start pos (9 bits)
	_EPD_writeData(0x00);
	_EPD_writeData(0xF9); // end pos (9 bits) (249)
	_EPD_writeData(0x00);

	_EPD_writeCommand(0x3C); // border waveform
	_EPD_writeData(0x05);

	// loading waveform LUT
	_EPD_writeCommand(0x18);
	_EPD_writeData(0x80);
	_EPD_writeCommand(0x22);
	_EPD_writeData(0xf7);
	_EPD_writeCommand(0x20);
	delay(1);
	while(digitalRead(EPD_BUSY_PIN) == HIGH){
		delay(1);
	}

	// write image in ram
	_EPD_writeCommand(0x4e);
	_EPD_writeData(0x00);
	_EPD_writeCommand(0x4f);
	_EPD_writeData(0x00);
	_EPD_writeData(0x00);
	_EPD_writeCommand(0x24);
	_EPD_writeData(bw_buf, EPD_BUF_SIZE);
	//_EPD_writeCommand(0x26); // use red ram for lulz (display doesnt have red pixels)
	_EPD_writeCommand(0x0c); // softstart setting
	_EPD_writeData(0x8b);
	_EPD_writeData(0x9c);
	_EPD_writeData(0x96);
	_EPD_writeData(0x0f);
	_EPD_writeCommand(0x22);
	_EPD_writeData(0xf7);
	_EPD_writeCommand(0x20);
	delay(1);
	_EPD_stallBusy();
}

void _EPD_writeCommand(uint8_t command){
	SPI.beginTransaction(_EPD_spiSettings);
	digitalWrite(EPD_DC_PIN, LOW);
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
	digitalWrite(EPD_CS_PIN, LOW);
	for(size_t i = 0; i<count; i++){
		SPI.transfer(data[i]);
	}
	digitalWrite(EPD_CS_PIN, HIGH);
	digitalWrite(EPD_DC_PIN, LOW);
	SPI.endTransaction();
}

void _EPD_powerOff(void){
	_EPD_writeCommand(0x10);
	_EPD_writeData(0x01);
}

void _EPD_drawPixel(int16_t x, int16_t y, bool black){
	if (x<0 || x >= EPD_WIDTH || y<0 || y >= EPD_HEIGHT){
		return;
	}

	uint16_t i = y * EPD_BYTES_PER_ROW + (x / 8);
	uint8_t mask = 0x80 >> (x % 8);

	if (black) {
		bw_buf[i] &= ~mask;   // 0 = black
	} else {
		bw_buf[i] |= mask;    // 1 = white
	}
}

void _EPD_stallBusy(void){
	while(digitalRead(EPD_BUSY_PIN) == HIGH){
		delay(1);
	}
}

void _EPD_fullRefresh(void){
	_EPD_writeCommand(0x22);
	_EPD_writeData(0xf7);
	_EPD_writeCommand(0x20);
	_EPD_stallBusy();
}

// updates display based on contents loaded in buffer
void _EPD_updateRam(){
	_EPD_writeCommand(0x4e);
	_EPD_writeData(0x00);
	_EPD_writeCommand(0x4f);
	_EPD_writeData(0x00);
	_EPD_writeData(0x00);
	_EPD_writeCommand(0x24);
	_EPD_writeData(bw_buf, EPD_BUF_SIZE);
}

void drawRectWithPixels(int x, int y, int w, int h) {
	// top and bottom
	for (int xx = x; xx < x + w; xx++) {
		_EPD_drawPixel(xx, y, true);           // top edge
		_EPD_drawPixel(xx, y + h - 1, true);   // bottom edge
	}

	// left and right
	for (int yy = y; yy < y + h; yy++) {
		_EPD_drawPixel(x, yy, true);           // left edge
		_EPD_drawPixel(x + w - 1, yy, true);   // right edge
	}
}

void gotoSleep(unsigned long time)
{
  // shutdown when time reaches SLEEPING_DELAY ms
  if ((time>SLEEPING_DELAY))
  {
    // to reduce power consumption when sleeping, turn off all your LEDs (and other power hungry devices)
    digitalWrite(LED_BUILTIN, LOW);                     

    // setup your wake-up pins.
    //pinMode(WAKE_LOW_PIN,  INPUT_PULLUP_SENSE);    // this pin (WAKE_LOW_PIN) is pulled up and wakes up the feather when externally connected to ground.
    //pinMode(WAKE_HIGH_PIN, INPUT_PULLDOWN_SENSE);  // this pin (WAKE_HIGH_PIN) is pulled down and wakes up the feather when externally connected to 3.3v.
 
    // power down nrf52.
    sd_power_system_off();                              // this function puts the whole nRF52 to deep sleep (no Bluetooth).  If no sense pins are setup (or other hardware interrupts), the nrf52 will not wake up.
  } 
}

void setup(){
	pinMode(13, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	pinMode(LED_BUILTIN, OUTPUT);
	_EPD_initDisplay();
	//_EPD_powerOff();
	//delay(1000);
	//digitalWrite(13, LOW);

	//_EPD_initDisplay();
	for(size_t i = 0; i <= 50; i++){
		//_EPD_drawPixel(i+20, i+20, true);

	}

	drawRectWithPixels(10, 10, 20, 20);
	_EPD_updateRam();
	_EPD_fullRefresh();
	
}
void loop(){
	gotoSleep(millis());
	//Serial.println("Hello world!");
}
