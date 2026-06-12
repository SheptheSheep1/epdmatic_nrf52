#include <Arduino.h>
#include "variant.h"
#include "wiring_constants.h"
#include <SPI.h>
#include "epdmatic_nrf52.h"
#include "test_bitmap.h"
#include "gfxfont.h"
#include "TomThumb.h"

// pins
#define SLEEPING_DELAY 5000


void setup(void);
void loop(void);


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

	//_EPD_drawRectWithPixels(10, 10, 20, 20);
	_EPD_drawBitmap1Bit(20, 20, icon_fajr_16x16, 16, 16, true);
	_EPD_drawBitmap1Bit(40, 20, icon_dhuhr_16x16, 16, 16, true);
	_EPD_drawBitmap1Bit(60, 20, icon_asr_16x16, 16, 16, true);
	_EPD_drawBitmap1Bit(80, 20, icon_maghrib_16x16, 16, 16, true);
	_EPD_drawBitmap1Bit(100, 20, icon_isha_16x16, 16, 16, true);
	EPD_drawText(150, 20, "what is up", &TomThumb, true);
	_EPD_updateRam();
	_EPD_fullRefresh();
	
}
void loop(){
	gotoSleep(millis());
	//Serial.println("Hello world!");
}
