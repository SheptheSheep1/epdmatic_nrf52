#include <cstdint>
#include <stdbool.h>
#include <nrf.h>
#include <nrfx.h>
#include "epdmatic_nrf52_br.c"
#include "nrf_gpio.h"
#include <nrfx_spim.h>
#include <nrfx_systick.h>
#include <nrf_spim.h>
//void epd_nrf__hw_make_io(){}
//
//
#define EPD_CS_PIN 1
#define EPD_BUSY_PIN 2
#define EPD_RST_PIN 3
#define EPD_DC_PIN 4

bool epd_nrf_busy_read(void* ctx);
void epd_nrf_cs_write(void* ctx, bool high);
void epd_nrf_dc_write(void* ctx, bool high);
void epd_nrf_delay_ms(void* ctx, uint32_t ms);
void epd_nrf_lock(void* ctx);
void epd_nrf_unlock(void* ctx);
int epd_nrf_spi_write(void* ctx, const uint8_t* data, size_t len);

static nrfx_spim_t spim_inst = NRFX_SPIM_INSTANCE(NRF_SPIM_INST_GET(SPIM_INST_IDX));

typedef struct{
	nrfx_spim_t spi;
	uint32_t cs_pin;
	uint32_t dc_pin;
	uint32_t rst_pin;
	uint32_t busy_pin;
} epd_nrfx_t;

static epd_nrfx_t epd_hw = {
	.spi = NRFX_SPIM2_INST_IDX,
	.cs_pin = EPD_CS_PIN,
	.dc_pin = EPD_DC_PIN,
	.rst_pin = EPD_RST_PIN,
	.busy_pin = EPD_BUSY_PIN
}

static epd_io_t epd_io = {
	.ctx = &epd_hw,
	.busy_read = epd_nrf_busy_read,
	.cs_write = epd_nrf_cs_write,
	.dc_write = epd_nrf_dc_write,
	.delay_ms = epd_nrf_delay_ms,
	.lock = epd_nrf_lock,
	.unlock = epd_nrf_unlock,
	.spi_write = 
};

bool epd_nrf_busy_read(void* ctx){
	epd_nrfx_t* epd_nrf = ctx;
	return ((nrf_gpio_pin_read(epd_nrf->busy_pin)) ? 1 : 0);
}

void epd_nrf_cs_write(void* ctx, bool high){
	epd_nrfx_t* epd_nrf = ctx;
	nrf_gpio_pin_write(epd_nrf->cs_pin, high);
	return;
}

void epd_nrf_dc_write(void* ctx, bool high){
	epd_nrfx_t* epd_nrf = ctx;
	nrf_gpio_pin_write(epd_nrf->dc_pin, high);
	return;
}

void epd_nrf_delay_ms(void* ctx, uint32_t ms){
	//epd_nrfx_t* epd_nrf = ctx;
	nrfx_systick_delay_ms(ms);
	return;
}

void epd_nrf_lock(void* ctx){
	asm("nop");
}

void epd_nrf_unlock(void* ctx){
	asm("nop");
}

int epd_nrf_spi_write(void* ctx, const uint8_t* data, size_t len){
	epd_nrfx_t* epd_nrf = ctx;
}
