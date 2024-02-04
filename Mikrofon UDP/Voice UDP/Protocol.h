/*
 * Protocol.h
 *
 * Created: 2019-01-12 15:59:32
 *  Author: Kajetan
 */ 


#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "Ethernet/UDP.h"
#include "buttons.h"

//MAKRA zwracajace nr. portu w zaleznosci od identyfikatora urzadzenia (np. M1,R2,A2) (podajemy 2 znak id (cyfrê) np. '1', '3')

// Dla radia Rx
#define GET_R_RE_PORT(x)	(8000 + (x) - '0')				 
#define GET_R_AN_PORT(x)	(9000 + (x) - '0')	
#define GET_R_AUDIO_PORT(x) (10000 + (x) - '0')	

// Dla wzmacniacza Ax
#define GET_A_RE_PORT(x)	(8100 + (x) - '0')	
#define GET_A_AN_PORT(x)	(9100 + (x) - '0')	
#define GET_A_AUDIO_PORT(x)	(10100 + (x) - '0')	

// Dla mikrofonu Mx
#define GET_M_RE_PORT(x)	(8200 + (x) - '0')	
#define GET_M_AN_PORT(x)	(9200 + (x) - '0')	


/*  
								dlugoœæ nag³ówku, pola nadawcy odbiorcy itp.
*/
#define PROTOCOL_HEADER_SIZE		6
#define PROTOCOL_PACKET_MIN_SIZE	PROTOCOL_HEADER_SIZE

#define PROTOCOL_NN_START			0
#define PROTOCOL_OO_START			2
#define PROTOCOL_TT_START			4
#define PROTOCOL_DATA_START			PROTOCOL_HEADER_SIZE

// makro zamieniaj¹ce pierwsze dwa bajty bufora na liczbê 16 bit
#define PACK_16_BUF(b) (   (uint16_t)     ( * ((uint8_t*)(b)) | ( *(((uint8_t*)(b)) + 1) << 8 ) )   )

// makro zamieniaj¹ce 2 znaki na liczbê 16 bit
#define PACK_16(a,b) ((uint16_t)((a) | ((b)<<8)))

// TT - typ pakietu
#define TT_RE PACK_16('R','E')	// Rozkaz
#define TT_AN PACK_16('A','N')	// Odpowiedz
#define TT_SE PACK_16('S','E')	// Ustaw

/*
		struktura opisuj¹ca stan transmisji
*/
typedef struct{
	
	bool RE_Sended;		// Czy wyslano komende T=1
	bool AN_Received;	// Czy odebrano odpowiedz
	bool TransmisionON;	// Czy trwa transmisja glosu
	Socket_t Socket;	// Gniazdo steruj¹ce aktualnej transmisji	 
	Button btn;			// Ktory przycisk rozpoczal transmisje
	
}TransmissionState_t;

extern TransmissionState_t TransmissionState;

extern Socket_t btn1Socket;	
extern Socket_t btn2Socket;
extern Socket_t btn3Socket;

void protocol_init();
void protocol_process();


void transmission_start(Socket_t s, uint8_t *OO);
void transmission_stop(Socket_t s, uint8_t *OO);

#endif /* PROTOCOL_H_ */