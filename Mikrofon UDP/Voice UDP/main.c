#include <atmel_start.h>
#include <atomic.h>
#include <stdio.h>
#include <util/delay.h>
#include "Ethernet/UDP.h"
#include "nvmctrl_basic.h"
#include "NodeConfig.h"
#include "VoiceProcessing.h"
#include "PT2257.h"
#include "buttons.h"
#include "Protocol.h"

/*
				Obs³uga przyciskow
*/
void BTN1_pressed();
void BTN1_released();
void BTN2_pressed();
void BTN2_released();
void BTN3_pressed();
void BTN3_released();


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
	buttons_init();

	buttons_set_on_pressed_callback(BTN1, BTN1_pressed);
	buttons_set_on_released_callback(BTN1, BTN1_released);
	
	buttons_set_on_pressed_callback(BTN2, BTN2_pressed);
	buttons_set_on_released_callback(BTN2, BTN2_released);
	
	buttons_set_on_pressed_callback(BTN3, BTN3_pressed);
	buttons_set_on_released_callback(BTN3, BTN3_released);
	
	/* Replace with your application code */
	while (1) {
		UDP_process();			// UDP background process
		voice_process();		// voice background process		
		buttons_process();		// buttons background process	
		protocol_process();		// protocol background process	
	}
}


/*
				Obs³uga przycisku 1
*/
void BTN1_pressed(){

	if(!TransmissionState.TransmisionON){
		TransmissionState.TransmisionON = true;
		TransmissionState.btn = BTN1;
		TransmissionState.Socket = btn1Socket;
		transmission_start(btn1Socket, ConfigButtons.btn1);
		TransmissionState.RE_Sended = true;
	}
}

void BTN1_released(){

	if(TransmissionState.TransmisionON && TransmissionState.btn == BTN1){
		TransmissionState.TransmisionON = false;
		transmission_stop(btn1Socket, ConfigButtons.btn1);
	}
}


/*
				Obs³uga przycisku 2
*/
void BTN2_pressed(){
	if(!TransmissionState.TransmisionON){
		TransmissionState.TransmisionON = true;
		TransmissionState.btn = BTN2;
		TransmissionState.Socket = btn2Socket;
		transmission_start(btn2Socket, ConfigButtons.btn2);
		TransmissionState.RE_Sended = true;
	}
}

void BTN2_released(){
	if(TransmissionState.TransmisionON && TransmissionState.btn == BTN2){
		TransmissionState.TransmisionON = false;
		transmission_stop(btn2Socket, ConfigButtons.btn2);
	}
}


/*
				Obs³uga przycisku 3
*/
void BTN3_pressed(){
	if(!TransmissionState.TransmisionON){
		TransmissionState.TransmisionON = true;
		TransmissionState.btn = BTN3;
		TransmissionState.Socket = btn3Socket;
		transmission_start(btn3Socket, ConfigButtons.btn3);
		TransmissionState.RE_Sended = true;
	}
}

void BTN3_released(){
	if(TransmissionState.TransmisionON && TransmissionState.btn == BTN3){
		TransmissionState.TransmisionON = false;
		transmission_stop(btn3Socket, ConfigButtons.btn3);
	}
}