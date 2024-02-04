/*
 * NodeConfig.c
 *
 * Created: 2018-10-29 16:11:22
 *  Author: Kajetan
 */ 

#include "NodeConfig.h"
#include "Ethernet/UDP.h"
#include "PT2257.h"
#include "Ethernet/socket.h"

void	configSocket_OnReceived(Socket_t s, uint16_t size);		// Callback do odbioru pakietów

/*

							Ustawienia w RAM (Settings) i wart poczatkowe w FLASH

*/ 


const VoiceAmpVolumes_t PROGMEM	VoiceAmpVolumesInit = {					// Amp volumes
	.ADC_amp = 6,
	.DAC_amp = 6
};
VoiceAmpVolumes_t	VoiceAmpVolumes; 


const IPConfigInfo_t PROGMEM IPConfigInfoInit = {						// Device IP config
	.DeviceIP	= {.bytes = {192, 168, 2, 200}},
	.GatewayIP	= {.bytes = {192, 168, 2, 99}},
	.SubnetMask = {.bytes = {255, 255, 255, 0}}
};
IPConfigInfo_t		IPConfigInfo;


const VoiceEndPoints_t PROGMEM	VoiceEndPointsInit = {					// Voice Endpoints
	.local = {
		.ip = {.bytes = {0,0,0,0}},
		.port = 11001
	},
	.remote = {
		.ip = {.bytes = {255,255,255,255}},
		.port = 11002
	}
};
VoiceEndPoints_t VoiceEndPoints;

const ConfigPassword_t PROGMEM ConfigPasswordInit = {					// Password
	.bytes = { 1, 2, 3, 4}
};
ConfigPassword_t	ConfigPassword;


const ConfigEndPoints_t PROGMEM	ConfigEndPointsInit = {					// Config Endpoints
	.local = {
		.ip = {.bytes = {0,0,0,0}},
		.port = 10000
	},
	.remote = {
		.ip = {.bytes = {255,255,255,255}},
		.port = 10000
	}
};
ConfigEndPoints_t ConfigEndPoints;


const EncryptionKey_t PROGMEM EncryptionKeyInit = {						// Encryption key
	.bytes = {0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80 ,0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0}
};
EncryptionKey_t EncryptionKey;


const ConfigGx_t PROGMEM ConfigGxInit = {								// ConfigGx

	.board = {BOARD_TYPE, BOARD_NUM}
		
};
ConfigGx_t ConfigGx;


/*
					Uzyte gniazda UDP
*/ 
extern Socket_t voiceSocket;					// Gniazdo do przesy³u dzwieku
extern Socket_t terminalSocket;					// Terminal UDP	  
Socket_t configSocket = SOCKET_ERR;				// Gniazdo do konfiguracji

extern volatile uint8_t buffer[128];
	 
	 
/*

									Init NodeConfig module

*/
void	configuration_init(void){
	
	// setup Settings
	if(!EEPROM_getEncryptionKey(&EncryptionKey)){	
		FLASH_getEncryptionKey(&EncryptionKey);
		EEPROM_setEncryptionKey(&EncryptionKey);
	}
		
	if(!EEPROM_getConfigPassword(&ConfigPassword)){
		FLASH_getConfigPassword(&ConfigPassword);
		EEPROM_setConfigPassword(&ConfigPassword);
	}
		
	if(!EEPROM_getVoiceAmpVolumes(&VoiceAmpVolumes)){
		FLASH_getVoiceAmpVolumes(&VoiceAmpVolumes);
		EEPROM_setVoiceAmpVolumes(&VoiceAmpVolumes);
	}
		
	if(!EEPROM_getVoiceEndPoints(&VoiceEndPoints)){
		FLASH_getVoiceEndPoints(&VoiceEndPoints);
		EEPROM_setVoiceEndPoints(&VoiceEndPoints);
	}
	
	if(!EEPROM_getConfigEndPoints(&ConfigEndPoints)){
		FLASH_getConfigEndPoints(&ConfigEndPoints);	
		EEPROM_setConfigEndPoints(&ConfigEndPoints);
	}
	
	if(!EEPROM_getConfigGx(&ConfigGx)){
		FLASH_getConfigGx(&ConfigGx);
		EEPROM_setConfigGx(&ConfigGx);
	}
	
	// Otwieramy gniazdo konfiguracji
	configSocket = UDP_openSocket(ConfigEndPoints.local.port, &ConfigEndPoints.remote);
	UDP_setOnReceiveCallback(configSocket, configSocket_OnReceived);
	
	//configuration_factory_reset();		//<<<	Kiedy wszystko sie z***ie
	
}


/*
									Factory Reset
*/
void soft_reset(void)
{
	asm("cli");
	asm("jmp 0x0000");	// Reset vector
}

void	configuration_factory_reset(){
	// Resetujemy wszystkie ustawienia
	EEPROM_resetEncryptionKey();
	EEPROM_resetConfigPassword();
	EEPROM_resetVoiceAmpVolumes();
	EEPROM_resetConfigEndPoints();
	EEPROM_resetVoiceEndPoints();
	EEPROM_resetIPConfigInfo();
	EEPROM_resetConfigGx();
	
	// Resetujemy CPU
	soft_reset();	
}


/*
			Pobiera wartosc rekordu spod podanego adresu, zwraca false jeœli pusty rekord
			Format rekordu w EEPROM :	addr		<IS_SET (BYTE)>
										addr+1		<RECORD_DATA (BYTE[])>
*/
bool EEPROM_getRecord(eeprom_adr_t addr, void *buf, uint16_t size){
	
	uint8_t is_set = 0;
	FLASH_0_read_eeprom_block(addr, &is_set, sizeof(uint8_t));
	if(is_set == EEPROM_SET){
		FLASH_0_read_eeprom_block(addr + 1, buf, size);
		return true;
	}else{
		return false;
	}
}


/*
			Ustawia wartosc rekordu pod danym adresem
*/
void EEPROM_setRecord(eeprom_adr_t addr, void *buf, uint16_t size){
	
	uint8_t is_set = EEPROM_SET;
	FLASH_0_write_eeprom_block(addr, &is_set, sizeof(uint8_t));
	FLASH_0_write_eeprom_block(addr + 1, buf, size);
	
}


/*
			Resetuje rekord (<addr>IS_SET = EEPROM_RESET)
*/
void EEPROM_resetRecord(eeprom_adr_t addr){
	
	uint8_t is_set = EEPROM_RESET;
	FLASH_0_write_eeprom_block(addr, &is_set, sizeof(uint8_t));	
}


/*
			Pobiera wartoœæ pocz¹tkow¹ (fabryczn¹) danego ustawienia
*/
inline void FLASH_getRecord(flash_adr_t addr, void *buf, uint16_t size){	
	while(size--)
		*(uint8_t*)(buf++) = pgm_read_byte(addr++);
}


/*

									UDP config socket interpreter

*/
static void	sendACK(){					// Send ACK WORD
	uint16_t ack = CONFIG_ACK;
	UDP_sendPacket(configSocket, &ack, sizeof(uint16_t));		// Send via UDP
}


static void	sendNACK(){					// Send NACK WORD (access denied / bad command)
	uint16_t nack = CONFIG_NACK;
	UDP_sendPacket(configSocket, &nack, sizeof(uint16_t));		// Send via UDP
}


static void	sendACK_WithResponse(void * data, uint16_t size){	// Send ACK with Response (Max 128B), using common buffer
	uint16_t ack = CONFIG_ACK;
	memcpy(buffer, &ack, 2);		// Copy ACK
	memcpy(buffer + 2, data, size);	// Copy response
	UDP_sendPacket(configSocket, buffer, size + sizeof(ack));	// Send via UDP
}


static inline bool check_password(void * buf){				// check password bytes
	return ! memcmp(&buf[CONFIG_PACKET_PASS], &ConfigPassword, sizeof(ConfigPassword_t));
}


static inline bool check_initial_password(void * buf){		// check password bytes (initial pass)
	ConfigPassword_t init_pass;
	FLASH_getConfigPassword(&init_pass);
	return ! memcmp(&buf[CONFIG_PACKET_PASS], &init_pass, sizeof(ConfigPassword_t));
}


static inline uint16_t getConfigCMD(uint8_t *buf){	// gets command word from UDP receive packet buffer
	return (uint16_t) ( buf[CONFIG_PACKET_CMD] + (buf[CONFIG_PACKET_CMD + 1] << 8) );	// Get command WORD
}


void configSocket_OnReceived(Socket_t s, uint16_t size){
	
	uint16_t bytes_readed = UDP_receivePacket(s, buffer, 128);	
	
	if(bytes_readed >= CONFIG_PACKET_MIN_SIZE){
		
		// Check if Factory reset CMD 		
		if (check_initial_password(buffer)){		// Factory reset command need initial password
			if(getConfigCMD(buffer) == CMD_FACTORY_RESET){
				printf("CMD_FACTORY_RESET\n");
				// do factory reset . . .
				sendACK();
				configuration_factory_reset();		// Reset CPU . . .
				return;
			}	
		}
		
		// Other commands 
		if(check_password(buffer)){
		
			switch( getConfigCMD(buffer) ){
			
				case CMD_TERMINAL_ECHO:		
			
					if( bytes_readed == CONFIG_PACKET_MIN_SIZE){
						printf("CMD_TERMINAL_ECHO\n");
						printf("Terminal connection OK\n");
						print_boot_info();	
						sendACK();
					}
					break;
			
				case CMD_CHECK_CONNECTION:		
					
					if( bytes_readed == CONFIG_PACKET_MIN_SIZE){	
						printf("CMD_CHECK_CONNECTION\n");				
						sendACK();					
					}
					break;
			
				case CMD_READ_IP_CONFIG:
	
					if( bytes_readed == CONFIG_PACKET_MIN_SIZE){
						printf("CMD_READ_IP_CONFIG\n");	
						sendACK_WithResponse(&IPConfigInfo, sizeof(IPConfigInfo_t));
					}				
					break;	
				
				case CMD_WRITE_IP_CONFIG:
			
					if( bytes_readed == (CONFIG_PACKET_MIN_SIZE + sizeof(IPConfigInfo_t)) ){
						printf("CMD_WRITE_IP_CONFIG\n");	
						memcpy(&IPConfigInfo, buffer + CONFIG_PACKET_DATA, sizeof(IPConfigInfo_t));
						printIPConfig(&IPConfigInfo);					
						EEPROM_setIPConfigInfo(&IPConfigInfo);
						sendACK();		
						soft_reset();		
					}				
					break;			
				
				case CMD_READ_CONFIG_ENDPOINTS:

					if( bytes_readed == CONFIG_PACKET_MIN_SIZE ){
						printf("CMD_READ_CONFIG_ENDPOINTS\n");
						sendACK_WithResponse(&ConfigEndPoints, sizeof(ConfigEndPoints_t));
					}
					break;
								
				case CMD_WRITE_CONFIG_ENDPOINTS:
					
					if( bytes_readed == (CONFIG_PACKET_MIN_SIZE + sizeof(ConfigEndPoints_t)) ){
						printf("CMD_WRITE_CONFIG_ENDPOINTS\n");
						memcpy(&ConfigEndPoints, buffer + CONFIG_PACKET_DATA, sizeof(ConfigEndPoints_t));
						printEP(&ConfigEndPoints.local);
						printEP(&ConfigEndPoints.remote);
						EEPROM_setConfigEndPoints(&ConfigEndPoints);
						sendACK();
						soft_reset();	
					}
					break;
				
				case CMD_READ_VOICE_AMP_VOLUMES:
						
					if( bytes_readed == CONFIG_PACKET_MIN_SIZE + 0 ){
						printf("CMD_READ_VOICE_AMP_VOLUMES\n");	
						sendACK_WithResponse(&VoiceAmpVolumes, sizeof(VoiceAmpVolumes_t));				
					}
					break;
			
				case CMD_WRITE_VOICE_AMP_VOLUMES:
								
					if( bytes_readed == (CONFIG_PACKET_MIN_SIZE + sizeof(VoiceAmpVolumes_t)) ){
						printf("CMD_WRITE_VOICE_AMP_VOLUMES\n");	
						memcpy(&VoiceAmpVolumes, buffer + CONFIG_PACKET_DATA, sizeof(VoiceAmpVolumes_t));
						printf("ADC amp: %d%%\n", VoiceAmpVolumes.ADC_amp);
						printf("DAC amp: %d%%\n", VoiceAmpVolumes.DAC_amp);
					
						// Update amplifier attenuations
						PT2257_setLAttenuation(VoiceAmpVolumes.ADC_amp);
						PT2257_setRAttenuation(VoiceAmpVolumes.DAC_amp);
					
						// Update EEPROM setting
						EEPROM_setVoiceAmpVolumes(&VoiceAmpVolumes);
						
						sendACK();
					}
				
					break;	
				
				case CMD_READ_ENCRYPTION_KEY:
					printf("CMD_READ_ENCRYPTION_KEY\n");					
					if( bytes_readed == CONFIG_PACKET_MIN_SIZE ){													
						sendACK_WithResponse(&EncryptionKey, sizeof(EncryptionKey_t));						
					}
					break;
						
				case CMD_WRITE_ENCRYPTION_KEY:
					printf("CMD_WRITE_ENCRYPTION_KEY\n");						
					if( bytes_readed == (CONFIG_PACKET_MIN_SIZE + sizeof(EncryptionKey_t)) ){
						memcpy(&EncryptionKey, &buffer[CONFIG_PACKET_DATA], sizeof(EncryptionKey_t));
						EEPROM_setEncryptionKey(&EncryptionKey);
						updateEncryptionKeyEx();
						sendACK();
						soft_reset();	
					}
						
					break;
					
				case CMD_SET_CONFIG_PASSWORD:
					printf("CMD_SET_CONFIG_PASSWORD\n");								
					if( bytes_readed == (CONFIG_PACKET_MIN_SIZE + sizeof(ConfigPassword_t)) ){
						memcpy(&ConfigPassword, &buffer[CONFIG_PACKET_DATA], sizeof(ConfigPassword_t));
						printf("%d %d %d %d\n", ConfigPassword.bytes[0], ConfigPassword.bytes[1], ConfigPassword.bytes[2], ConfigPassword.bytes[3]);
						EEPROM_setConfigPassword(&ConfigPassword);
						sendACK();
					}
								
					break;
						
				case CMD_READ_GX_CONFIG:
					if( bytes_readed == CONFIG_PACKET_MIN_SIZE ){
						printf("CMD_READ_GX_CONFIG\n");
						sendACK_WithResponse(&ConfigGx, sizeof(ConfigGx_t));
					}
					break;

				case CMD_WRITE_GX_CONFIG:
					if(bytes_readed ==(CONFIG_PACKET_MIN_SIZE + sizeof(ConfigGx_t))){
						printf("CMD_WRITE_GX_CONFIG\n");
						
						memcpy(&ConfigGx, &buffer[CONFIG_PACKET_DATA], sizeof(ConfigGx_t));
						EEPROM_setConfigGx(&ConfigGx);
						
						sendACK();
						
						soft_reset();
					}
					break;
																		
				default:
					sendNACK();	// Invalid command				
			}	
		}
		else{	
			sendNACK();		// Access Denied
		}
	}	
}


/*
									UDP boot info
*/
extern MACAddress_t mac;

void print_boot_info(){
	
	printf("IP Config:\t\t");
	printIPConfig(&IPConfigInfo);
	
	printf("Voice Dest:\t\t");
	printEP(&VoiceEndPoints.remote);
	
	printf("Voice Src:\t\t");
	printEP(&VoiceEndPoints.local);
	
	printf("Config Dest:\t\t");
	printEP(&ConfigEndPoints.remote);
	
	printf("Config Src:\t\t");
	printEP(&ConfigEndPoints.local);
	
	printf("Generated MAC:\t\t");
	printMAC(&mac);
	
	printf("\nBoard ID: %c%c\n", ConfigGx.board[0], ConfigGx.board[1]);
	
}