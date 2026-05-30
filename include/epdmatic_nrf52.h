#ifndef EPDMATIC_NRF52
#define EPDMATIC_NRF52

#include <stdlib.h> // needed for size_t ig
#include <stdint.h>

// constants
#define EPD_RST_PIN (32 + 04)
#define EPD_CLK_PIN (0 + 24)
#define EPD_DIN_PIN (0 + 22)
#define EPD_CS_PIN (32 + 00)
#define EPD_DC_PIN (00 + 11)
#define EPD_BUSY_PIN (32 + 06)
#define EPD_NC_PIN (0)

#define EPD_PANEL_WIDTH 122
#define EPD_PANEL_HEIGHT 250

// Set to 90 or 270 if the panel is mounted/displaying sideways.
#ifndef EPD_ROTATION
#define EPD_ROTATION 90
#endif

#if EPD_ROTATION == 90 || EPD_ROTATION == 270
#define EPD_WIDTH EPD_PANEL_HEIGHT
#define EPD_HEIGHT EPD_PANEL_WIDTH
#elif EPD_ROTATION == 0 || EPD_ROTATION == 180
#define EPD_WIDTH EPD_PANEL_WIDTH
#define EPD_HEIGHT EPD_PANEL_HEIGHT
#else
#error "EPD_ROTATION must be 0, 90, 180, or 270"
#endif

#define EPD_BUF_SIZE (((EPD_PANEL_WIDTH + 7) / 8) * EPD_PANEL_HEIGHT )
#define EPD_BYTES_PER_ROW ((EPD_PANEL_WIDTH + 7) / 8)

#define _EPD_spiSettings (SPISettings(4000000, MSBFIRST, SPI_MODE0)) // NRF_SPIM_MODE_0, 4MHz

void _EPD_HWReset(void);
void _EPD_SWReset(void);
void _EPD_initDisplay(void);
void _EPD_writeCommand(uint8_t command);
void _EPD_writeData(uint8_t data);
void _EPD_writeData(const uint8_t* buf, size_t n);
void _EPD_powerOff(void);
void _EPD_stallBusy(void);
void _EPD_fullRefresh(void);
void _EPD_updateRam(void);
void _EPD_drawRectWithPixels(int x, int y, int width, int height);
void _EPD_drawBitmap1Bit(int16_t x, int16_t y, const uint8_t* bitmap, int16_t w, int16_t h, bool black);

#endif // !EPDMATIC_NRF52
