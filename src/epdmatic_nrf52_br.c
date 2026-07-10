#include <nrfx.h>

#define _EPD_DC_ACTIVE 1 // active-high
#define _EPD_CS_ACTIVE 0 // active-low chip-select (obviously) insert finger pointing up emoji
#define _EPD_BUSY_ACTIVE 0 // active-low busy (read from epd)

typedef struct {
	void* ctx;
	int (*spi_write)(void* ctx, const uint8_t* data, size_t len);
	//int (*spi_write_byte)(void* ctx, const uint8_t data); // don't want to add unless actually necessary
	void (*cs_write)(void* ctx, bool high);
	void (*dc_write)(void* ctx, bool high);
	bool (*busy_read)(void* ctx);
	void (*delay_ms)(void* ctx, uint32_t ms);
	void (*lock)(void* ctx);
	void (*unlock)(void* ctx);
} epd_io_t;

void epd_swReset(epd_io_t* io){
	io->lock(io->ctx);
	io->spi_write_byte(io->ctx, 0x12);
	io->unlock(io->ctx);
}

void epd_stallBusy(epd_io_t* io){

}

void epd_sendCmd(epd_io_t* io, const uint8_t cmd){
	io->lock(io->ctx);
	io->dc_write(io->ctx, !(_EPD_DC_ACTIVE));
	io->cs_write(io->ctx, (_EPD_CS_ACTIVE));
	//__writeByte(io, cmd);
	io->spi_write(io->ctx, &cmd, 1);
	io->cs_write(io->ctx, !(_EPD_CS_ACTIVE));
	io->unlock(io->ctx);
}

void epd_sendData(epd_io_t* io, const uint8_t* data, size_t len){
	io->lock(io->ctx);
	io->unlock(io->ctx);
}
