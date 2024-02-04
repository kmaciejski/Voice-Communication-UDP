/*
 * socket.c
 *
 * Created: 2018-10-18 18:14:37
 *  Author: Kajetan
 */ 

#include "socket.h"

volatile EndPoint_t	lastPacketEP = {0};		// Adres ostatnio odebranego pakietu
volatile uint16_t	lastPacketSize = 0;		// Rozmiar ostatnio odebranego pakietu


//
//		Zamyka gniazdo
//
void socketClose(SOCKET s)
{
	execCmdSn(s, Sock_CLOSE);
	writeSnIR(s, 0xFF);
}


//
//		Otwiera gniazdo
//
bool socketOpen(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag)
{
	if ((protocol == MR_TCP) 
	|| (protocol == MR_UDP) 
	|| (protocol == MR_IPRAW) 
	|| (protocol == MR_MACRAW) 
	|| (protocol == MR_PPPOE))
	{
		socketClose(s);
		writeSnMR(s, protocol | flag);
		if (port != 0) {
			writeSnPORT(s, port);
		}
		else {
			return false;	// Invalid port number.
		}

		execCmdSn(s, Sock_OPEN);
		
		return true;
	}

	return false;
}


//
//		Zapisuje dane do TX bufora
//
uint16_t socketBufferData(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len)
{
	if (len <= getTXFreeSize(s))
	{
		send_data_processing_offset(s, offset, buf, len);
	}
	return len;
}


//
//		Wysy³a TX bufor jako pakiet UDP
//
bool socketSendUDP(SOCKET s)
{
	execCmdSn(s, Sock_SEND);
	
	while ( (readSnIR(s) & IR_SEND_OK) != IR_SEND_OK )
	{
		if (readSnIR(s) & IR_TIMEOUT)
		{
			writeSnIR(s, (IR_SEND_OK | IR_TIMEOUT));
			return false;
		}
	}
	
	writeSnIR(s, IR_SEND_OK);

	/* Sent ok */
	return true;
}


//
//		Czyta pakiet (po procedurze socketParsePacket), zwraca ilosc odczytanych bajtow
//
uint16_t	socketReadPacket(SOCKET s, uint8_t *buf, uint16_t len){
	
	if(len > lastPacketSize)
		len = lastPacketSize;
		
	recv_data_processing(s, buf, len );
	
	return len;
	
}


//
//		Pobiera rozmiar, ip i port nadawcy pakietu, zwraca false jesli nie ma pakietow
//
bool socketParsePacket(SOCKET s)
{
	if (getRXReceivedSize(s) >= 8)
	{
		//HACK - hand-parse the UDP packet using TCP recv method
		uint8_t headerBuf[8];
		
		// Odczytujemy naglowek
		recv_data_processing(s, headerBuf, 8 );
			
		// Zapisujemy dane z naglowka UDP
		memcpy(lastPacketEP.ip.bytes, headerBuf, 4);
		lastPacketEP.port = headerBuf[4];
		lastPacketEP.port = (lastPacketEP.port << 8) + headerBuf[5];
		lastPacketSize = headerBuf[6];
		lastPacketSize = (lastPacketSize << 8) + headerBuf[7];

		return true;
	}
	// There aren't any packets available
	return false;
}