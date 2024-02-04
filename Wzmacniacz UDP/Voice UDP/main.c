#include <atmel_start.h>
#include <atomic.h>
#include <stdio.h>
#include <util/delay.h>
#include "Ethernet/UDP.h"
#include "nvmctrl_basic.h"
#include "NodeConfig.h"
#include "VoiceProcessing.h"
#include "PT2257.h"
#include "Protocol.h"

// Terminal socket
Socket_t terminalSocket = SOCKET_ERR;	// Terminal UDP

// Terminal EndPoint (PC) (for testing)
#define terminalPort 7000
EndPoint_t terminalDestEP = {
	.ip		= {192, 168, 2, 100},
	.port	= terminalPort
};

// Bufffer
volatile uint8_t buffer[128] = {0};

// printf support for UDP terminal
#if defined(__GNUC__)

int UDP_terminal_printCHAR(char character, FILE *stream)
{
	UDP_sendPacket(terminalSocket, &character, 1);
	return 0;
}

FILE UDP_terminal_stream = FDEV_SETUP_STREAM(UDP_terminal_printCHAR, NULL, _FDEV_SETUP_WRITE);

#elif defined(__ICCAVR__)

int putchar(int outChar)
{
	char character = outChar;
	UDP_sendPacket(terminalSocket, &character, 1);
	return outChar;
}
#endif

MACAddress_t mac;

void	generateMACAddress(MACAddress_t *mac){
	mac->bytes[0] = SIGROW.SERNUM4;
	mac->bytes[1] = SIGROW.SERNUM5;
	mac->bytes[2] = SIGROW.SERNUM6;
	mac->bytes[3] = SIGROW.SERNUM7;
	mac->bytes[4] = SIGROW.SERNUM8;
	mac->bytes[5] = SIGROW.SERNUM9;	
}

extern volatile uint16_t send_cnt, recv_cnt;

/*
						Entry point
*/
int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();	

	PT2257_init();
	
	if( ! EEPROM_getIPConfigInfo(&IPConfigInfo) )
		FLASH_getIPConfigInfo(&IPConfigInfo);
	
	generateMACAddress(&mac);
	
	UDP_Init(&mac, &IPConfigInfo);	// Init UDP

	terminalSocket = UDP_openSocket(terminalPort, &terminalDestEP);
	
	// UDP terminal init
	#if defined(__GNUC__)
	stdout = &UDP_terminal_stream;
	#endif

	configuration_init();
	
	printf("UDP terminal on port %d\n\n", terminalPort);
	
	print_boot_info();
	
	voice_init();	
	protocol_init();	
	
	/* Replace with your application code */
	while (1) {
		UDP_process();			// UDP background process
		protocol_process();		// protocol background process	
	}
}