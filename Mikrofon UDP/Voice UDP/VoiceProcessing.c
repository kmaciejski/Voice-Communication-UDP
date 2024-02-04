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
	
	UDP_getLocalPort(TransmissionState.Socket, &EP.port);
	EP.port += 1000;	// Z 9x0x na 10x0x
	Port = EP.port;
	
	// Otwieramy gniazdo dzwieku
	voiceSocket = UDP_openSocket(Port, &EP);

}
/*
		voice init & AES module init
*/
void	voice_init(){
		
	// aktualizujemy klucz szyfrujacy
	updateEncryptionKeyEx();
}


/*
		voice thread  -  encrypt & send data via UDP
*/
void	voice_process(){
	
	if(S_FIFO.packet_count > 2){				// Jesli przynajmniej 2 pakiety s¹ gotowe to wysy³amy pakiet
		
		#ifdef AES_ENCRYPTION
		aesCipher(aes_key_ex, get_send_packet_buf() + 1);			// Szyfrujemy 4 paczki dŸwiêku
		aesCipher(aes_key_ex, get_send_packet_buf() + 16 + 1);
		aesCipher(aes_key_ex, get_send_packet_buf() + 32 + 1);
		aesCipher(aes_key_ex, get_send_packet_buf() + 48 + 1);
		#endif
		
		UDP_sendPacket(voiceSocket, get_send_packet_buf() + 1, VOICE_DATA_SIZE);
		switch_send_packet_buf();					// Przelaczamy bufory nadawania
	}
	
}


/*
				TCA OVF ~ (8KHz)
*/
ISR(TCA0_OVF_vect)
{
	/* Insert your TCA overflow interrupt handling code */

	/* The interrupt flag has to be cleared manually */
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}


/*
				ADC READY ~ (8KHz)
*/
ISR(ADC0_RESRDY_vect)
{
	/* Insert your ADC result ready interrupt handling code here */
	
	uint8_t sample8 = (ADC0.RES >> 5);	// (ADC.RES / 8) >> 2
		
	write_sample(&sample8);
	
	/* The interrupt flag has to be cleared manually */
	ADC0.INTFLAGS = ADC_RESRDY_bm;
}