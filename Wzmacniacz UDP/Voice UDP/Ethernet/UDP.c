/*
 * UDP.c
 *
 * Created: 2018-10-19 16:00:59
 *  Author: Kajetan
 */ 

#include "UDP.h"
#include "socket.h"
#include <string.h>

//
//			Socket descriptor struct
//
typedef struct{

	UDP_OnReceiveCallback_t cb;	// set if != null	
	uint16_t		port;
	EndPoint_t		destEP;
	bool			available;
	
}SocketDescriptor_t;

SocketDescriptor_t Sockets[MAX_SOCK_NUM] = {0};

// Check if the socket is available
static bool check_socket(Socket_t s){
	if(s < MAX_SOCK_NUM){
		if(Sockets[s].available)
			return true;
	}	
	return false;
}

/*

								Basic UDP API procedures

*/

//
//		Inicjalizacja biblioteki UDP
//
bool	UDP_Init( MACAddress_t *mac, IPConfigInfo_t *ipconfig ){
	
	MACAddress_t get_mac;
	IPConfigInfo_t get_ipconfig;
	
	init();	// W5500 init
	
	// Clear Sockets array
	memset(Sockets, 0, sizeof(Sockets));
	
	// Check the W5500 version (always 0x04)
	if(getVersion() != 0x04)
		return false;
		
	// IP config
	setMACAddress(mac->bytes);	
	UDP_setIPConfig(ipconfig);
	
	// Verify IP Config
	getMACAddress(&get_mac);
	UDP_getIPConfig(&get_ipconfig);

	if( (!memcmp(mac, &get_mac, sizeof(MACAddress_t))) 
		&& (!memcmp(ipconfig, &get_ipconfig, sizeof(IPConfigInfo_t))))
		return true;
	
	return false;
}


//
//		sprawdza w petli glownej czy przychdza pakiety, czyta naglowek UDP i wywoluje odpowiednie callbacki
//
void	UDP_process(void){
	
	uint8_t sock;
	uint16_t size;
	
	for(sock = 0; sock < MAX_SOCK_NUM; sock++){
		if(Sockets[sock].available){
			size = UDP_available(sock);
			if(size > 0 && Sockets[sock].cb)
				Sockets[sock].cb(sock, size);
		}
	}
}


//
//		otwiera gniazdo
//
Socket_t UDP_openSocket(uint16_t srcPort, EndPoint_t *destEP){
	
	uint8_t i;
	Socket_t sock = 0xFF;
	
	for(i = 0; i < MAX_SOCK_NUM; i++){
		
		if( !Sockets[i].available ){

			uint8_t s = readSnSR(i);
			if ( s == SR_CLOSED || s == SR_FIN_WAIT ) {
				
				if( socketOpen(i, MR_UDP, srcPort, 0) ){	// Otwieramy gniazdo
					writeSnDIPR(i, destEP->ip.bytes);
					writeSnDPORT(i, destEP->port);			// Ustawiamy adres docelowy					
					sock = i;
					Sockets[i].available = true;
					Sockets[i].port = srcPort;
					memcpy(&Sockets[i].destEP, destEP, sizeof(EndPoint_t));
					break;
				}		
			}
		}		
	}
	return sock;
}


//
//		zamyka gniazdo
//
void UDP_closeSocket(Socket_t s){
	if(check_socket(s)){
		socketClose(s);
		Sockets[s].available = false;
	}
}


//
//		Pobiera port i IP nadawcy (wywolujemy po UDP_available())
//
void UDP_getSenderEndPoint(EndPoint_t *ep){
	memcpy(ep, &lastPacketEP, sizeof(EndPoint_t));
}


//
//		Wysyla pakiet
//
bool UDP_sendPacket(Socket_t s, void *data, uint16_t size){
	if(check_socket(s)){
		uint16_t bytes_writed = 0;
		bytes_writed = socketBufferData(s, 0, (uint8_t*)data, size);
		
		if(bytes_writed == size){
			
			if(socketSendUDP(s))
				return true;		
		}
	}
	return false;
}


//
//		Sprawdza czy jest dostepny pakiet i zwraca jego rozmiar
//
uint16_t	UDP_available(Socket_t s){
	if(check_socket(s)){
		if( socketParsePacket(s)){
			return lastPacketSize;	
		}
	}
	return 0;
}


//
//		Czyta pakiet (wywolujemy po UDP_available())
//
uint16_t	UDP_receivePacket(Socket_t s, void *data, uint16_t size){
	
	uint16_t bytes_readed = 0;
	
	if(check_socket(s)){
			
		if (lastPacketSize > 0)
		{
			bytes_readed = socketReadPacket(s, data, size);					
		}
	}
	return bytes_readed;
}


//
//		Ustawia callback przy odbieraniu danych
//
void		UDP_setOnReceiveCallback(Socket_t s, UDP_OnReceiveCallback_t cb){
	if(check_socket(s))
		Sockets[s].cb = cb;
}


//
//		Pobiera i ustawia IP i port zdalny dla danego gniazda
//
void		UDP_getRemoteEP(Socket_t s, EndPoint_t *ep){
	if(check_socket(s)){
		memcpy(ep, &Sockets[s].destEP, sizeof(EndPoint_t));
	}
}		
				
void		UDP_setRemoteEP(Socket_t s, EndPoint_t *ep){
	if(check_socket(s)){
		if(ep){
			writeSnDIPR(s, ep->ip.bytes);
			writeSnDPORT(s, ep->port);
		}
	}
}					


//
//		Pobiera port lokalny dla danego gniazda (ignoruje ustawianie IP)
//
void		UDP_getLocalPort(Socket_t s, uint16_t *port){
	if(check_socket(s)){
		*port = Sockets[s].port;
	}	
}						
			


//
//		Pobiera konfiguracje IP
//
void		UDP_getIPConfig(IPConfigInfo_t *IPConfigInfo){
	
	getIPAddress(IPConfigInfo->DeviceIP.bytes);
	getGatewayIp(IPConfigInfo->GatewayIP.bytes);
	getSubnetMask(IPConfigInfo->SubnetMask.bytes);
	
}


//
//		Zapisuje konfiguracje IP
//
void		UDP_setIPConfig(IPConfigInfo_t *IPConfigInfo){
	
	setIPAddress(IPConfigInfo->DeviceIP.bytes);
	setGatewayIp(IPConfigInfo->GatewayIP.bytes);
	setSubnetMask(IPConfigInfo->SubnetMask.bytes);
	
}