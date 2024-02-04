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

Socket_t btn1Socket			= SOCKET_ERR;					// Gniazdo przypisane do przycisku 1
Socket_t btn2Socket			= SOCKET_ERR;					// Gniazdo przypisane do przycisku 2
Socket_t btn3Socket			= SOCKET_ERR;					// Gniazdo przypisane do przycisku 3

TransmissionState_t TransmissionState;

/*
			Komendy rozpoczecia i zakonczenia nadawania
*/
void	transmission_start(Socket_t s, uint8_t *OO){		// Podajemy gniazdo na ktore wysylamy komende i wskaznik do bufora okreslajacego pole OO (adresata)
	
	uint8_t buf[16];
	
	memcpy(buf, ConfigGx.board, 2);	// Kopiujemy pole nadawcy
	memcpy(buf+2, OO, 2);			// Kopiujemy pole odbiorcy
	memcpy(buf+4, "RET=\1", 5);		// Kopiujemy pole typu komendy i ci¹g T=\1
	
	UDP_sendPacket(s, buf, 9);		// Wysylamy
	
}

void	transmission_stop(Socket_t s, uint8_t *OO){		// Podajemy gniazdo na ktore wysylamy komende i wskaznik do bufora okreslajacego pole OO (adresata)
	
	uint8_t buf[16];
	
	memcpy(buf, ConfigGx.board, 2);	// Kopiujemy pole nadawcy
	memcpy(buf+2, OO, 2);			// Kopiujemy pole odbiorcy
	memcpy(buf+4, "RET=\0", 5);		// Kopiujemy pole typu komendy i ci¹g T=\1
	
	UDP_sendPacket(s, buf, 9);		// Wysylamy
	
}


/*
			porty sterowania przypisane do przycisków
*/
void buttons_ports_init(){

	uint16_t Port;		// Port na ktorym nasluchujemy
	EndPoint_t EP = {		// IP i port na ktory nadajemy
		.ip = {255,255,255,255}		// nadajemy w broadcast
	};
	
	// Zamykamy gniazda w razie potrzeby
	if(btn1Socket != SOCKET_ERR)
		UDP_closeSocket(btn1Socket);
	if(btn2Socket != SOCKET_ERR)
		UDP_closeSocket(btn2Socket);
	if(btn3Socket != SOCKET_ERR)
		UDP_closeSocket(btn3Socket);
	
	
	// Przycisk 1
	if(ConfigButtons.btn1[0] == 'A'){		// Wzmacniacz
		Port = GET_A_AN_PORT(ConfigButtons.btn1[1]);
		EP.port = GET_A_RE_PORT(ConfigButtons.btn1[1]);
	}else if(ConfigButtons.btn1[0] == 'R'){	// Radio
		Port = GET_R_AN_PORT(ConfigButtons.btn1[1]);
		EP.port = GET_R_RE_PORT(ConfigButtons.btn1[1]);		
	}

	btn1Socket = UDP_openSocket(Port, &EP);

	// Przycisk 2
	if(ConfigButtons.btn2[0] == 'A'){		// Wzmacniacz
		Port = GET_A_AN_PORT(ConfigButtons.btn2[1]);
		EP.port = GET_A_RE_PORT(ConfigButtons.btn2[1]);
		}else if(ConfigButtons.btn2[0] == 'R'){	// Radio
		Port = GET_R_AN_PORT(ConfigButtons.btn2[1]);
		EP.port = GET_R_RE_PORT(ConfigButtons.btn2[1]);
	}

	btn2Socket = UDP_openSocket(Port, &EP);
	
	// Przycisk 3
	if(ConfigButtons.btn3[0] == 'A'){		// Wzmacniacz
		Port = GET_A_AN_PORT(ConfigButtons.btn3[1]);
		EP.port = GET_A_RE_PORT(ConfigButtons.btn3[1]);
		}else if(ConfigButtons.btn3[0] == 'R'){	// Radio
		Port = GET_R_AN_PORT(ConfigButtons.btn3[1]);
		EP.port = GET_R_RE_PORT(ConfigButtons.btn3[1]);
	}

	btn3Socket = UDP_openSocket(Port, &EP);

	UDP_setOnReceiveCallback(btn1Socket, buttonsSocket_OnReceived);		// Ustawiamy procedure do odbioru danych
	UDP_setOnReceiveCallback(btn2Socket, buttonsSocket_OnReceived);
	UDP_setOnReceiveCallback(btn3Socket, buttonsSocket_OnReceived);
	
}


/*
			Inicjalizacja protoko³u sterowania
*/
void protocol_init(){

	uint16_t protocolPort = GET_M_RE_PORT(ConfigGx.board[1]);
	
	EndPoint_t protocolEP = {
		.ip = {255,255,255,255},
		.port = GET_M_AN_PORT(ConfigGx.board[1])
	};
	
	protocolSocket = UDP_openSocket(protocolPort, &protocolEP);
	
	UDP_setOnReceiveCallback(protocolSocket, protocolSocket_OnReceived);
	
	buttons_ports_init();	// Inicjalizacja portów sterowania przypisanych do przycisków
	
}


/*
			odebrano dane z portu sterowania przypisanego do przycisku
*/
void	buttonsSocket_OnReceived(Socket_t s, uint16_t size){	// Odebrano dane z jednego z gniazd przypisanego do przycisku

	uint8_t		OO[2];	// Odbiorca
	
	UDP_receivePacket(s, buffer, 128);

	if(size == PROTOCOL_PACKET_MIN_SIZE + 3){	// sprawdzamy rozmiar

		// Pobieramy pole odbiorcy
		OO[0] = buffer[PROTOCOL_OO_START];
		OO[1] = buffer[PROTOCOL_OO_START + 1];

		if(!memcmp(OO, ConfigGx.board, 2)){	// Sprawdzamy czy pakiet dotyczy tego mikrofonu

			if(TransmissionState.TransmisionON){// Sprawdzamy flagê transmisji
				
				if(!memcmp(buffer + PROTOCOL_TT_START, "ANT=\1", 3))	// Sprawdzamy sk³adnie "ANT=1"
					TransmissionState.AN_Received = true;
			}
		}	
	}
}


/*
			odebrano dane z portu sterowania przypisanego do tego mikrofonu
*/
void	protocolSocket_OnReceived(Socket_t s, uint16_t size){
	
	uint16_t	TT;
	uint8_t		OO[2];	// Odbiorca
	
	UDP_receivePacket(s, buffer, 128);
	
	if(size == PROTOCOL_PACKET_MIN_SIZE + 4){
		
		// Pobieramy pole odbiorcy
		OO[0] = buffer[PROTOCOL_OO_START];
		OO[1] = buffer[PROTOCOL_OO_START + 1];
		
		if(!memcmp(OO, ConfigGx.board, 2)){	// Sprawdzamy czy pakiet dotyczy tego mikrofonu
		
			TT = PACK_16_BUF(buffer + PROTOCOL_TT_START);
		
			if(TT == TT_SE){
			
				//Ustawiamy przypisania do przycisków
			
				if(buffer[PROTOCOL_DATA_START + 1] == '='){					// Sprawdzamy skladnie (czy jest znak '=')
					
					if(buffer[PROTOCOL_DATA_START + 2] != 'A'				// Sprawdzamy sk³adnie adresata Ax lub Rx
						&& buffer[PROTOCOL_DATA_START + 2] != 'R')
						return;
						
					if(buffer[PROTOCOL_DATA_START + 3] > '9' || buffer[PROTOCOL_DATA_START + 3] < '1')	// Sprawdzamy nr. radia / wzmacniacza (1 - 9)
						return;
					
					if(buffer[PROTOCOL_DATA_START] == 1){		// Przycisk 1
						memcpy(ConfigButtons.btn1, buffer + PROTOCOL_DATA_START + 2, 2);
					}else if(buffer[PROTOCOL_DATA_START] == 2){	// Przycisk 2
						memcpy(ConfigButtons.btn2, buffer + PROTOCOL_DATA_START + 2, 2);
					}else if(buffer[PROTOCOL_DATA_START] == 3){	// Przycisk 3
						memcpy(ConfigButtons.btn3, buffer + PROTOCOL_DATA_START + 2, 2);
					}else{
						return;	
					}
					
					EEPROM_setConfigButtons(&ConfigButtons);	// Zapisujemy konfiguracje w eeprom
					buttons_ports_init();						// otwieramy porty przypisane do przyciskow
				}
				
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
	
	if(TransmissionState.TransmisionON){
		if(TransmissionState.RE_Sended && TransmissionState.AN_Received){
			TransmissionState.RE_Sended = false;	// Czyscimy flage wyslania komendy
			TransmissionState.AN_Received = false;
				
			switch(TransmissionState.btn){	// Rozpoczêcie transmisji na okreslonym porcie przez przycisk
			
				case BTN1:
					LED1_ON_set_level(true);				
					break;	
				
				case BTN2:
					LED2_ON_set_level(true);				
					break;
				
				case BTN3:
					LED3_ON_set_level(true);				
					break;	
			}
			
			voice_port_init();
			TC0_start();
				
		}
		
	}else{		// Jesli transmisja nie trwa gasimy ledy, czyscimy flagi i wy³¹czamy timer
		LED1_ON_set_level(false);
		LED2_ON_set_level(false);
		LED3_ON_set_level(false);
		TransmissionState.RE_Sended = false;
		TransmissionState.AN_Received = false;
		TC0_stop();
	}
	
}