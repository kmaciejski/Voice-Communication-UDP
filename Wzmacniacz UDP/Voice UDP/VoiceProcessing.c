/*
 * VoiceProcessing.c
 *			Operacje odbierania / wysylania dzwieku po UDP
 * Created: 2018-11-08 00:19:40
 *  Author: Kajetan
 */ 

#include "VoiceProcessing.h"
#include "NodeConfig.h"
#include "Ethernet/UDP.h"
#include "FIFO.h"
#include "Protocol.h"
#include "aes.h"
#include <driver_init.h>
#include <compiler.h>
#include <atomic.h>

#define AES_ENCRYPTION

void	voiceSocket_OnReceived(Socket_t s, uint16_t size);


// voice sockets
Socket_t voiceSocket		= SOCKET_ERR;					// Gniazdo do przesy³u dzwieku


// AES key ex. array
uint8_t aes_key_ex	[176];									// klucz rozszerony (stworzony dla algorytmu szybkiego szyfrowania AES 128bit)


/*
		update key ex
*/
void	updateEncryptionKeyEx(){		// Aktualizuje klucz rozszerzony (dla szybkiego szyfrowania)
	aesKeyExpand(EncryptionKey.bytes, aes_key_ex);
}

void	voice_port_init(){
	
	uint16_t Port;			// Port na ktorym nasluchujemy	
	EndPoint_t EP = {		// IP i port na ktory nadajemy
		.ip = {255,255,255,255}		// nadajemy w broadcast
	};
	
	if(voiceSocket!=SOCKET_ERR)
		UDP_closeSocket(voiceSocket);
		
	Port = EP.port = GET_A_AUDIO_PORT(ConfigGx.board[1]);
	
	// Otwieramy gniazdo dzwieku
	voiceSocket = UDP_openSocket(Port, &EP);
	UDP_setOnReceiveCallback(voiceSocket, voiceSocket_OnReceived);
}
/*
		voice init & AES module init
*/
void	voice_init(){
	
	voice_port_init();
	// aktualizujemy klucz szyfrujacy
	updateEncryptionKeyEx();
	
}


/*
		Receiving sound packets via UDP
*/
void	voiceSocket_OnReceived(Socket_t s, uint16_t size){
	
	if(size == VOICE_DATA_SIZE){
		UDP_receivePacket(s, get_receive_packet_buf() + 1, VOICE_DATA_SIZE);		// Odczytujemy pakiet do bufora FIFO
			
		#ifdef AES_ENCRYPTION	
		aesInvCipher(aes_key_ex, get_receive_packet_buf() + 1);						// Deszyfrujemy 4 paczki dŸwiêku
		aesInvCipher(aes_key_ex, get_receive_packet_buf() + 16 + 1);
		aesInvCipher(aes_key_ex, get_receive_packet_buf() + 32 + 1);
		aesInvCipher(aes_key_ex, get_receive_packet_buf() + 48 + 1);
		#endif

		switch_receive_packet_buf();			// Przelaczamy bufory odbioru
	
	}		
	
}


/*
				TCA OVF ~ (8KHz)
*/
ISR(TCA0_OVF_vect)
{
	/* Insert your TCA overflow interrupt handling code */
	uint8_t sample;
	read_sample(&sample);
	
	DAC_0_set_output(sample);
	/* The interrupt flag has to be cleared manually */
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}


/*
				ADC READY ~ (8KHz)
*/
ISR(ADC0_RESRDY_vect)
{
	/* Insert your ADC result ready interrupt handling code here */
	
	/* The interrupt flag has to be cleared manually */
	ADC0.INTFLAGS = ADC_RESRDY_bm;
}