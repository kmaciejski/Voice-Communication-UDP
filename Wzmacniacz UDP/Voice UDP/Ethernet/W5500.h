/*
 * W5500.h
 *
 * Created: 2018-10-18 16:21:35
 *  Author: Kajetan
 */ 


#ifndef W5500_H_
#define W5500_H_

#include <atmel_start.h>
#include "IP.h"
// ----------------------------------
// W5500 Definitions & Data types
// ----------------------------------

typedef uint8_t SOCKET;		// Socket type W5500 registers - Level

#define MAX_SOCK_NUM 8

typedef enum{
	MR_CLOSE  = 0x00,
	MR_TCP    = 0x01,
	MR_UDP    = 0x02,
	MR_IPRAW  = 0x03,
	MR_MACRAW = 0x04,
	MR_PPPOE  = 0x05,
	MR_ND     = 0x20,
	MR_MULTI  = 0x80
}SnMR;

typedef enum{
	Sock_OPEN      = 0x01,
	Sock_LISTEN    = 0x02,
	Sock_CONNECT   = 0x04,
	Sock_DISCON    = 0x08,
	Sock_CLOSE     = 0x10,
	Sock_SEND      = 0x20,
	Sock_SEND_MAC  = 0x21,
	Sock_SEND_KEEP = 0x22,
	Sock_RECV      = 0x40
}SockCMD;

typedef enum{
   IR_SEND_OK = 0x10,
   IR_TIMEOUT = 0x08,
   IR_RECV    = 0x04,
   IR_DISCON  = 0x02,
   IR_CON     = 0x01
}SnIR;

typedef enum{
   SR_CLOSED      = 0x00,
   SR_INIT        = 0x13,
   SR_LISTEN      = 0x14,
   SR_SYNSENT     = 0x15,
   SR_SYNRECV     = 0x16,
   SR_ESTABLISHED = 0x17,
   SR_FIN_WAIT    = 0x18,
   SR_CLOSING     = 0x1A,
   SR_TIME_WAIT   = 0x1B,
   SR_CLOSE_WAIT  = 0x1C,
   SR_LAST_ACK    = 0x1D,
   SR_UDP         = 0x22,
   SR_IPRAW       = 0x32,
   SR_MACRAW      = 0x42,
   SR_PPPOE       = 0x5F
}SnSR;

typedef enum{
   IP   = 0,
   ICMP = 1,
   IGMP = 2,
   GGP  = 3,
   TCP  = 6,
   PUP  = 12,
   UDP  = 17,
   IDP  = 22,
   ND   = 77,
   RAW  = 255
}IPPROTO;


// -------------------------
// W5500 extern procs
// -------------------------

void		execCmdSn(SOCKET s, SockCMD _cmd);
uint16_t	getTXFreeSize(SOCKET s);
uint16_t	getRXReceivedSize(SOCKET s);
void		send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len);
void		send_data_processing(SOCKET s, const uint8_t *data, uint16_t len);
void		read_data(SOCKET s, volatile uint16_t src, volatile uint8_t *dst, uint16_t len);
void		recv_data_processing(SOCKET s, uint8_t *data, uint16_t len);


// -------------------------
// W5500 Basic procedures
// -------------------------

void	init	(void);
void	write_byte	(uint16_t addr, uint8_t cb, uint8_t data);
uint8_t read_byte	(uint16_t addr, uint8_t cb);
uint16_t write_buf(uint16_t addr, uint8_t cb, const uint8_t *buf, uint16_t len);
uint16_t read_buf(uint16_t addr, uint8_t cb, const uint8_t *buf, uint16_t len);


// -------------------
// W5500 Registers
// -------------------
 
#define __GP_REGISTER8(name, address)             \
static inline void write##name(uint8_t _data) { \
	write_byte(address, 0x04, _data);                  \
}                                               \
static inline uint8_t read##name() {            \
	return read_byte(address, 0x00);                   \
}

#define __GP_REGISTER16(name, address)            \
static void write##name(uint16_t _data) {       \
	write_byte(address,  0x04, _data >> 8);            \
	write_byte(address+1, 0x04, _data & 0xFF);         \
}                                               \
static uint16_t read##name() {                  \
	uint16_t res = read_byte(address, 0x00);           \
	res = (res << 8) + read_byte(address + 1, 0x00);   \
	return res;                                   \
}

#define __GP_REGISTER_N(name, address, size)      \
static uint16_t write##name(uint8_t *_buff) {   \
	return write_buf(address, 0x04, _buff, size);     \
}                                               \
static uint16_t read##name(uint8_t *_buff) {    \
	return read_buf(address, 0x00, _buff, size);      \
}

__GP_REGISTER8 (MR,     0x0000);    // Mode
__GP_REGISTER_N(GAR,    0x0001, 4); // Gateway IP address
__GP_REGISTER_N(SUBR,   0x0005, 4); // Subnet mask address
__GP_REGISTER_N(SHAR,   0x0009, 6); // Source MAC address
__GP_REGISTER_N(SIPR,   0x000F, 4); // Source IP address
__GP_REGISTER8 (IR,     0x0015);    // Interrupt
__GP_REGISTER8 (IMR,    0x0016);    // Interrupt Mask
__GP_REGISTER16(RTR,    0x0019);    // Timeout address
__GP_REGISTER8 (RCR,    0x001B);    // Retry count
__GP_REGISTER_N(UIPR,   0x0028, 4); // Unreachable IP address in UDP mode
__GP_REGISTER16(UPORT,  0x002C);    // Unreachable Port address in UDP mode
__GP_REGISTER8 (PHYCFGR,0x002E);    // PHY Configuration register, default value: 0b 1011 1xxx
__GP_REGISTER8 (VERSIONR,0x0039);   // W5500 Version register (always 0x04)
  
#undef __GP_REGISTER8
#undef __GP_REGISTER16
#undef __GP_REGISTER_N


// -----------------------
// W5500 Socket registers
// -----------------------

inline uint8_t readSn_byte(SOCKET _s, uint16_t _addr) {
	uint8_t cntl_byte = (_s<<5)+0x08;
	return read_byte(_addr, cntl_byte);
}

inline void writeSn_byte(SOCKET _s, uint16_t _addr, uint8_t _data) {
	uint8_t cntl_byte = (_s<<5)+0x0C;
	write_byte(_addr, cntl_byte, _data);
}

inline uint16_t readSn_buf(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
	uint8_t cntl_byte = (_s<<5)+0x08;
	return read_buf(_addr, cntl_byte, _buf, _len );
}

inline uint16_t writeSn_buf(SOCKET _s, uint16_t _addr, uint8_t *_buf, uint16_t _len) {
	uint8_t cntl_byte = (_s<<5)+0x0C;
	return write_buf(_addr, cntl_byte, _buf, _len);
}

#define __SOCKET_REGISTER8(name, address)                    \
static inline void write##name(SOCKET _s, uint8_t _data) { \
	writeSn_byte(_s, address, _data);                             \
}                                                          \
static inline uint8_t read##name(SOCKET _s) {              \
	return readSn_byte(_s, address);                              \
}

#define __SOCKET_REGISTER16(name, address)                   \
static void write##name(SOCKET _s, uint16_t _data) {       \
	writeSn_byte(_s, address,   _data >> 8);                      \
	writeSn_byte(_s, address+1, _data & 0xFF);                    \
}                                                          \
static uint16_t read##name(SOCKET _s) {                    \
	uint16_t res = readSn_byte(_s, address);                      \
	res = (res << 8) + readSn_byte(_s, address + 1);              \
	return res;                                              \
}

#define __SOCKET_REGISTER_N(name, address, size)             \
static uint16_t write##name(SOCKET _s, uint8_t *_buff) {   \
	return writeSn_buf(_s, address, _buff, size);                \
}                                                          \
static uint16_t read##name(SOCKET _s, uint8_t *_buff) {    \
	return readSn_buf(_s, address, _buff, size);                 \
}

__SOCKET_REGISTER8(SnMR,        0x0000)        // Mode
__SOCKET_REGISTER8(SnCR,        0x0001)        // Command
__SOCKET_REGISTER8(SnIR,        0x0002)        // Interrupt
__SOCKET_REGISTER8(SnSR,        0x0003)        // Status
__SOCKET_REGISTER16(SnPORT,     0x0004)        // Source Port
__SOCKET_REGISTER_N(SnDHAR,     0x0006, 6)     // Destination Hardw Addr
__SOCKET_REGISTER_N(SnDIPR,     0x000C, 4)     // Destination IP Addr
__SOCKET_REGISTER16(SnDPORT,    0x0010)        // Destination Port
__SOCKET_REGISTER16(SnMSSR,     0x0012)        // Max Segment Size
__SOCKET_REGISTER8(SnPROTO,     0x0014)        // Protocol in IP RAW Mode
__SOCKET_REGISTER8(SnTOS,       0x0015)        // IP TOS
__SOCKET_REGISTER8(SnTTL,       0x0016)        // IP TTL
__SOCKET_REGISTER16(SnTX_FSR,   0x0020)        // TX Free Size
__SOCKET_REGISTER16(SnTX_RD,    0x0022)        // TX Read Pointer
__SOCKET_REGISTER16(SnTX_WR,    0x0024)        // TX Write Pointer
__SOCKET_REGISTER16(SnRX_RSR,   0x0026)        // RX Free Size
__SOCKET_REGISTER16(SnRX_RD,    0x0028)        // RX Read Pointer
__SOCKET_REGISTER16(SnRX_WR,    0x002A)        // RX Write Pointer (supported?)

#undef __SOCKET_REGISTER8
#undef __SOCKET_REGISTER16
#undef __SOCKET_REGISTER_N


// ---------------------------
// W5500 Control procedures
// ---------------------------

inline uint8_t getVersion() {
	return readVERSIONR();
}

inline void getGatewayIp(uint8_t *_addr) {
	readGAR(_addr);
}

inline void setGatewayIp(uint8_t *_addr) {
	writeGAR(_addr);
}

inline void getSubnetMask(uint8_t *_addr) {
	readSUBR(_addr);
}

inline void setSubnetMask(uint8_t *_addr) {
	writeSUBR(_addr);
}

inline void getMACAddress(uint8_t *_addr) {
	readSHAR(_addr);
}

inline void setMACAddress(uint8_t *_addr) {
	writeSHAR(_addr);
}

inline void getIPAddress(uint8_t *_addr) {
	readSIPR(_addr);
}

inline void setIPAddress(uint8_t *_addr) {
	writeSIPR(_addr);
}

inline void setRetransmissionTime(uint16_t _timeout) {
	writeRTR(_timeout);
}

inline void setRetransmissionCount(uint8_t _retry) {
	writeRCR(_retry);
}

inline void setPHYCFGR(uint8_t _val) {
	writePHYCFGR(_val);
}

inline uint8_t getPHYCFGR() {
	return readPHYCFGR();
}

inline void swReset() {
	writeMR( (readMR() | 0x80) );
}

#endif /* W5500_H_ */