/*
 * W5500.c
 *
 * Created: 2018-10-18 16:20:10
 *  Author: Kajetan
 */ 

#include "W5500.h"
#include <atomic.h>
#include <util/delay.h>
#include <spi_basic.h>

/*
					SS control procedures
*/
static inline void setSS(void)		{ SS_set_level(false); }
static inline void resetSS(void)	{ SS_set_level(true); }


/*
					W5500 chip init proc
*/
void	init(void){
	
	SPI_0_enable();
	resetSS();
	
	uint8_t i;

	for(i=0; i < 10; i++){	// Wykonujemy 10x inicjalizacje dla pewnosci ;)
	
		_delay_ms(100);
	
		swReset();
	
		for (int i=0; i < MAX_SOCK_NUM; i++) {	// Ustawiamy rozmiary buforów gniazd na 2KB kazdy
			uint8_t cntl_byte = (0x0C + (i<<5));
			write_byte( 0x1E, cntl_byte, 2); //0x1E - Sn_RXBUF_SIZE
			write_byte( 0x1F, cntl_byte, 2); //0x1F - Sn_TXBUF_SIZE
		}
	}
	
	setRetransmissionTime(10);

	setRetransmissionCount(3);
	
}


// --------------------------------------
// W5500 read / write to registers
// --------------------------------------

/*
					write byte
*/
void	write_byte(uint16_t addr, uint8_t cb, uint8_t data){
	
	uint8_t addr_l = (addr & 0xFF);
	uint8_t addr_h = (addr >> 8);
	
	setSS();
		
	SPI_0_exchange_byte(addr_h);
	SPI_0_exchange_byte(addr_l);
	SPI_0_exchange_byte(cb);
	SPI_0_exchange_byte(data);
	
	resetSS();

}


/*
					read byte
*/
uint8_t read_byte(uint16_t addr, uint8_t cb)
{

	uint8_t addr_l = (addr & 0xFF);
	uint8_t addr_h = (addr >> 8);
	uint8_t data = 0;

	setSS();

	SPI_0_exchange_byte(addr_h);
	SPI_0_exchange_byte(addr_l);
	SPI_0_exchange_byte(cb);
	data = SPI_0_exchange_byte(0x00);
	
	resetSS();

	return data;
}


/*
					write buffer
*/
uint16_t write_buf(uint16_t addr, uint8_t cb, const uint8_t *buf, uint16_t len){
	
	
	uint8_t addr_l = (addr & 0xFF);
	uint8_t addr_h = (addr >> 8);
	
	//DISABLE_INTERRUPTS();
	
	setSS();
	
	SPI_0_exchange_byte(addr_h);
	SPI_0_exchange_byte(addr_l);
	SPI_0_exchange_byte(cb);	
	SPI_0_write_block(buf, len);
	
	resetSS();
	
	//ENABLE_INTERRUPTS();
		
	return len;	
}


/*
					read buffer
*/
uint16_t read_buf(uint16_t addr, uint8_t cb, const uint8_t *buf, uint16_t len){

	uint8_t addr_l = (addr & 0xFF);
	uint8_t addr_h = (addr >> 8);
	
	//DISABLE_INTERRUPTS();	
	setSS();
	SPI_0_exchange_byte(addr_h);
	SPI_0_exchange_byte(addr_l);
	SPI_0_exchange_byte(cb);
	SPI_0_read_block(buf, len);	
	resetSS();
	//ENABLE_INTERRUPTS();	

	return len;
}


// -------------------------
// W5500 Basic operations
// -------------------------

/*
					Socket - execute command
*/
void execCmdSn(SOCKET s, SockCMD _cmd) {
	// Send command to socket
	writeSnCR(s, _cmd);
	// Wait for command to complete
	while (readSnCR(s))
	;
}


/*
					Socket - get TX buffer free size
*/
uint16_t getTXFreeSize(SOCKET s)
{
	uint16_t val=0, val1=0;
	do {
		val1 = readSnTX_FSR(s);
		if (val1 != 0)
			val = readSnTX_FSR(s);
	}
	while (val != val1);
	return val;
}


/*
					Socket - get received bytes
*/
uint16_t getRXReceivedSize(SOCKET s)
{
	uint16_t val=0,val1=0;
	do {
		val1 = readSnRX_RSR(s);
		if (val1 != 0)
			val = readSnRX_RSR(s);
	}
	while (val != val1);
	return val;
}

/*
					Socket - send data to socket TX buffer
*/
void send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{
	uint16_t ptr = readSnTX_WR(s);
	uint8_t cntl_byte = (0x14+(s<<5));	//Sn TX buffer select
	ptr += data_offset;
	write_buf(ptr, cntl_byte, data, len);
	ptr += len;
	writeSnTX_WR(s, ptr);
}

void send_data_processing(SOCKET s, const uint8_t *data, uint16_t len)
{
	// This is same as having no offset in a call to send_data_processing_offset
	send_data_processing_offset(s, 0, data, len);
}


/*
					Socket - read data from RX buffer
*/
void read_data(SOCKET s, volatile uint16_t src, volatile uint8_t *dst, uint16_t len)
{
	uint8_t cntl_byte = (0x18+(s<<5));
	read_buf((uint16_t)src , cntl_byte, (uint8_t *)dst, len);
}

/*
					Socket - read data from RX buffer & update RX read pointer
*/
void recv_data_processing(SOCKET s, uint8_t *data, uint16_t len)
{
	uint16_t ptr;
	ptr = readSnRX_RD(s);

	read_data(s, ptr, data, len);

	ptr += len;
	writeSnRX_RD(s, ptr);
	
	execCmdSn(s, Sock_RECV);
}