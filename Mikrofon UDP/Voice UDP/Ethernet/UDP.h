/*
 * UDP.h
 *
 * Created: 2018-10-19 16:01:10
 *  Author: Kajetan
 */ 


#ifndef UDP_H_
#define UDP_H_

#include "IP.h"
#include "atmel_start.h"

typedef void (*UDP_OnReceiveCallback_t)(Socket_t s, uint16_t size);

/* init procedure (ip config & W5500 initialization) */
bool	UDP_Init( MACAddress_t *mac, IPConfigInfo_t *ipconfig );

/* put this function in main loop */
void	UDP_process(void);

/* Basic API procedures */
Socket_t	UDP_openSocket(uint16_t srcPort, EndPoint_t *destEP);				// Open new socket with remote EP
void		UDP_closeSocket(Socket_t s);										// Close socket
bool		UDP_sendPacket(Socket_t s, void *data, uint16_t size);				// Send packet to remote address & port
uint16_t	UDP_available(Socket_t s);											// gets number of received bytes for given socket 
uint16_t	UDP_receivePacket(Socket_t s, void *data, uint16_t size);			// gets packet bytes for given socket
void		UDP_setOnReceiveCallback(Socket_t s, UDP_OnReceiveCallback_t cb);	// set onReceive callback
void		UDP_getSenderEndPoint(EndPoint_t *ep);								// Pobiera ip i port nadawcy

/* Getters & Setters */
void		UDP_getRemoteEP(Socket_t s, EndPoint_t *ep);						//	Pobiera i ustawia IP i port zdalny 
void		UDP_setRemoteEP(Socket_t s, EndPoint_t *ep);						//	dla danego gniazda

void		UDP_getLocalPort(Socket_t s, uint16_t *port);						//	Pobiera i ustawia IP i port lokalny

void		UDP_getIPConfig(IPConfigInfo_t *IPConfigInfo);
void		UDP_setIPConfig(IPConfigInfo_t *IPConfigInfo);
	

#endif /* UDP_H_ */