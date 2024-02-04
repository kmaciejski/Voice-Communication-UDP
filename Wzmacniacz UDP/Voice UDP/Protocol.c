/*
 * Protocol.c
 *
 * Created: 2019-01-12 15:59:22
 *  Author: Kajetan
 */ 

#include "Protocol.h"
#include "NodeConfig.h"
#include "VoiceProcessing.h"

void	protocolSocket_OnReceived(Socket_t s, uint16_t size);
void	buttonsSocket_OnReceived(Socket_t s, uint16_t size);


Socket_t protocolSocket		= SOCKET_ERR;					// Gniazdo do komunikacji z PC (sterowanie / odpowiedzi)
extern volatile uint8_t buffer[128];

TransmissionState_t TransmissionState;

/*
			Inicjalizacja protoko³u sterowania
*/
void protocol_init(){

	uint16_t protocolPort = GET_A_RE_PORT(ConfigGx.board[1]);
	
	EndPoint_t protocolEP = {
		.ip = {255,255,255,255},
		.port = GET_A_AN_PORT(ConfigGx.board[1])
	};
	
	protocolSocket = UDP_openSocket(protocolPort, &protocolEP);
	UDP_setOnReceiveCallback(protocolSocket, protocolSocket_OnReceived);
	
	
}


/*
			odebrano dane z portu sterowania przypisanego do tego wzmacniacza
*/
void	protocolSocket_OnReceived(Socket_t s, uint16_t size){
	
	uint16_t	TT;
	uint8_t		OO[2];	// Odbiorca
	
	UDP_receivePacket(s, buffer, 128);
	
	if(size == PROTOCOL_PACKET_MIN_SIZE + 3){	// Pakiet T=x
		
		// Pobieramy pole odbiorcy
		OO[0] = buffer[PROTOCOL_OO_START];
		OO[1] = buffer[PROTOCOL_OO_START + 1];
		
		if(!memcmp(OO, ConfigGx.board, 2)){	// Sprawdzamy czy pakiet dotyczy tego mikrofonu
		
			TT = PACK_16_BUF(buffer + PROTOCOL_TT_START);
		
			if(TT == TT_RE){		// Sprawdzamy czy rozkaz RE
			
				if(memcmp(buffer + PROTOCOL_DATA_START, "T=", 2))	// Sprawdzamy sk³adnie "T="
					return;
					
				if(buffer[PROTOCOL_DATA_START + 2] == 0){		// Transmission OFF 
					TC0_stop();
				}
				else if(buffer[PROTOCOL_DATA_START + 2] == 1){	// Transmission ON 
					TC0_start();
				}
				else
					return;
										
				buffer[PROTOCOL_TT_START] = 'A';							// Typ pakietu - AN
				buffer[PROTOCOL_TT_START+1] = 'N';
				buffer[PROTOCOL_OO_START] = buffer[PROTOCOL_NN_START];		// Zamieniamy pola odbiorcy i nadawcy miejscami
				buffer[PROTOCOL_OO_START+1] = buffer[PROTOCOL_NN_START+1];
				buffer[PROTOCOL_NN_START] = ConfigGx.board[0];
				buffer[PROTOCOL_NN_START+1] = ConfigGx.board[1];
				
				UDP_sendPacket(s, buffer, size);							// Odsylamy odpowiedz
			
			}
		
		}
		
	}
	
}


/*
			Œledzenie stanu transmisji (czekanie na odpowiedz) i zapalanie diod LED
*/
void protocol_process(){
	//
	//if(TransmissionState.TransmisionON){
		//if(TransmissionState.RE_Sended && TransmissionState.AN_Received){
			//TransmissionState.RE_Sended = false;	// Czyscimy flage wyslania komendy
			//TransmissionState.AN_Received = false;
				//
			//switch(TransmissionState.btn){	// Rozpoczêcie transmisji na okreslonym porcie przez przycisk
			//
				//case BTN1:
					//LED1_ON_set_level(true);				
					//break;	
				//
				//case BTN2:
					//LED2_ON_set_level(true);				
					//break;
				//
				//case BTN3:
					//LED3_ON_set_level(true);				
					//break;	
			//}
			//
			//voice_port_init();
			//TC0_start();
				//
		//}
		//
	//}else{		// Jesli transmisja nie trwa gasimy ledy, czyscimy flagi i wy³¹czamy timer
		//LED1_ON_set_level(false);
		//LED2_ON_set_level(false);
		//LED3_ON_set_level(false);
		//TransmissionState.RE_Sended = false;
		//TransmissionState.AN_Received = false;
		//TC0_stop();
	//}
	
}