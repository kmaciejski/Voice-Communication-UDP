/*
 * IP.c
 *
 * Created: 2018-10-18 18:59:53
 *  Author: Kajetan
 */ 

#include "IP.h"
#include <stdio.h>

//static IPAddress_t rawIP = {
	//0, 0, 0, 0
//};

//
//		zwraca tablice reprezentujaca adres IP
//
//uint8_t * getRawIPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d){
	//rawIP[0] = a;
	//rawIP[1] = b;
	//rawIP[2] = c;
	//rawIP[3] = d;
	//return rawIP;
//}


//
//		Sprawdza adres IP
//
bool checkIPAddress(IPAddress_t *ip){
	if(ip->bytes[0] 
		&& ip->bytes[1] 
		&& ip->bytes[2] 
		&& ip->bytes[3])
	{
		
		if((ip->bytes[0] != 0xFF)
			&& (ip->bytes[1] != 0xFF)
			&& (ip->bytes[2] != 0xFF)
			&& (ip->bytes[3] != 0xFF))
			return true;
	}
	return false;
}


//
//		Wypisuje adres IP
//
void printIP(IPAddress_t *ip){
	
	printf("%d.%d.%d.%d",ip->bytes[0], ip->bytes[1], ip->bytes[2], ip->bytes[3]);	
	
}


//
//		Wypisuje adres MAC
//
void		printMAC(MACAddress_t *mac){
	
	printf("%02X:%02X:%02X:%02X:%02X:%02X",
		mac->bytes[0],
		mac->bytes[1],
		mac->bytes[2], 
		mac->bytes[3],
		mac->bytes[4],
		mac->bytes[5]
	);
	
}


//
//		Wypisuje punkt koncowy gniazda(IP + PORT)
//
void		printEP(EndPoint_t *ep){
	
	printf("IP: ");
	printIP(&ep->ip);
	printf("\tPORT: %d", ep->port);
	printf("\n");
	
}

//
//		Wypisuje konfiguracjê IP
//
void		printIPConfig(IPConfigInfo_t *IPConfigInfo){
	
	printf("IP: ");
	printIP(&IPConfigInfo->DeviceIP);
	printf("\nGateway IP: ");
	printIP(&IPConfigInfo->GatewayIP);
	printf("\nSubnet mask: ");
	printIP(&IPConfigInfo->SubnetMask);	
	printf("\n");	
}