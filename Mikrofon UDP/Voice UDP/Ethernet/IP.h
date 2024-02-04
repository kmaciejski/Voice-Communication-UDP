/*
 * IP.h
 *
 * Created: 2018-10-18 18:55:11
 *  Author: Kajetan
 */ 


#ifndef IP_H_
#define IP_H_

#include <atmel_start.h>

typedef struct{			// IP Address type
	uint8_t bytes[4];
}IPAddress_t;

typedef struct{			// MAC Address type
	uint8_t bytes[6];
}MACAddress_t;

typedef struct{			// EndPoint struct
	
	IPAddress_t ip;
	uint16_t port;
	
}EndPoint_t;

typedef struct{
	IPAddress_t DeviceIP;
	IPAddress_t GatewayIP;
	IPAddress_t SubnetMask;
}IPConfigInfo_t;

typedef uint8_t Socket_t;		// Socket type UDP Api - Level

#define SOCKET_ERR	 0xFF		// Invalid socket

//IPAddress_t* getRawIPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
void		printIP(IPAddress_t *ip);
void		printEP(EndPoint_t *ep);
void		printIPConfig(IPConfigInfo_t *IPConfigInfo);
void		printMAC(MACAddress_t *mac);

bool		checkIPAddress(IPAddress_t *ip);

#endif /* IP_H_ */