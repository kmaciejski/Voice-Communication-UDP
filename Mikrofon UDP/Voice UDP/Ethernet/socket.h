/*
 * socket.h
 *
 * Created: 2018-10-18 18:14:47
 *  Author: Kajetan
 */ 


#ifndef SOCKET_H_
#define SOCKET_H_

#include "W5500.h"
#include "IP.h"

extern volatile EndPoint_t	lastPacketEP;		// Adres ostatnio odebranego pakietu
extern volatile uint16_t	lastPacketSize;			// Rozmiar ostatnio odebranego pakietu

bool		socketOpen(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag);	// Otwiera gniazdo
void		socketClose(SOCKET s);																	// Zamyka gniazdo

uint16_t	socketBufferData(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len);	// Zapisuje dane do bufora TX

bool		socketSendUDP(SOCKET s);														// Wysyla dane z bufora TX

bool		socketParsePacket(SOCKET s);	// Sprawdza czy jest pakiet do odebrania, i odczytuje naglowek UDP
uint16_t	socketReadPacket(SOCKET s, uint8_t *buf, uint16_t len);	// Czyta pakiet UDP (bez naglowka)

#endif /* SOCKET_H_ */