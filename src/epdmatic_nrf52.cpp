#include <epdmatic_nrf52.h>
#include <Arduino.h>
#include <SPI.h>

uint8_t bw_buf[EPD_BUF_SIZE];

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

	SPI.setPins(EPD_NC_PIN, EPD_CLK_PIN, EPD_DIN_PIN);
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
	_EPD_writeData(((EPD_PANEL_WIDTH + 7) / 8) - 1); // end X ram pos addr
	
	_EPD_writeCommand(0x45); // set ram y addr
	_EPD_writeData(0x00); // start pos (9 bits)
	_EPD_writeData(0x00);
	_EPD_writeData((EPD_PANEL_HEIGHT - 1) & 0xFF); // end pos (9 bits)
	_EPD_writeData((EPD_PANEL_HEIGHT - 1) >> 8);

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

void _EPD_writeData(const uint8_t* buf, size_t n){
	SPI.beginTransaction(_EPD_spiSettings);
	digitalWrite(EPD_DC_PIN, HIGH);
	digitalWrite(EPD_CS_PIN, LOW);
	for(size_t i = 0; i < n; i++){
		SPI.transfer(buf[i]);
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

	int16_t panelX;
	int16_t panelY;

#if EPD_ROTATION == 0
	panelX = x;
	panelY = y;
#elif EPD_ROTATION == 90
	panelX = EPD_PANEL_WIDTH - 1 - y;
	panelY = x;
#elif EPD_ROTATION == 180
	panelX = EPD_PANEL_WIDTH - 1 - x;
	panelY = EPD_PANEL_HEIGHT - 1 - y;
#elif EPD_ROTATION == 270
	panelX = y;
	panelY = EPD_PANEL_HEIGHT - 1 - x;
#endif

	uint16_t i = panelY * EPD_BYTES_PER_ROW + (panelX / 8);
	uint8_t mask = 0x80 >> (panelX % 8); // bitwise shift to access proper bit

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
void _EPD_updateRam(void){
	_EPD_writeCommand(0x4e);
	_EPD_writeData(0x00);
	_EPD_writeCommand(0x4f);
	_EPD_writeData(0x00);
	_EPD_writeData(0x00);
	_EPD_writeCommand(0x24);
	_EPD_writeData(bw_buf, EPD_BUF_SIZE);
}

void _EPD_drawRectWithPixels(int x, int y, int w, int h) {
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

// draws bitmap arr
void _EPD_drawBitmap1Bit(int16_t x, int16_t y, const uint8_t* bitmap, int16_t w, int16_t h, bool black){
	uint16_t bytesPerBitmapRow = (w + 7) / 8;

	for (int16_t yy = 0; yy < h; yy++) {
		for (int16_t xx = 0; xx < w; xx++) {
			uint8_t byte = bitmap[yy * bytesPerBitmapRow + (xx / 8)];
			uint8_t mask = 0x80 >> (xx % 8);

			bool bitSet = byte & mask;

			if (bitSet) {
				_EPD_drawPixel(x + xx, y + yy, black);
			}
		}
	}
}
