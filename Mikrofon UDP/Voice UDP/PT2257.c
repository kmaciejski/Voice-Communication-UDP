/*
 * PT2257.c
 *
 * Created: 2018-11-03 18:05:02
 *  Author: Kajetan
 */ 
#include <util/delay.h>
#include "PT2257.h"
#include "NodeConfig.h"

extern VoiceAmpVolumes_t	VoiceAmpVolumes;

void PT2257_init(){
	_delay_ms(200);
// Update amplifier attenuations
	PT2257_setLAttenuation(VoiceAmpVolumes.ADC_amp);
	PT2257_setRAttenuation(VoiceAmpVolumes.DAC_amp);
}

uint8_t PT2257_level(uint8_t dB){
	
	if(dB>79) dB=79;
	
	uint8_t b = dB/10;  //get the most significant digit (eg. 79 gets 7)
	uint8_t a = dB%10;  //get the least significant digit (eg. 79 gets 9)
	
	b = b & 0b0000111; //limit the most significant digit to 3 bit (7)
	
	return (b<<4) | a; //return both numbers in one byte (0BBBAAAA)
}

void PT2257_setRAttenuation(uint8_t dB){
	
	uint8_t bbbaaaa = PT2257_level(dB);
	
	uint8_t aaaa = bbbaaaa & 0b00001111;
	uint8_t bbb = (bbbaaaa>>4) & 0b00001111;
	
	uint8_t data[2] = {		// Build WORD
		(EVC_R_10 | bbb),
		(EVC_R_1 | aaaa)
	};
	
	I2C_0_writeNBytes(PT2257_ADDR, data, 2);
	
}

void PT2257_setLAttenuation(uint8_t dB){
	
	uint8_t bbbaaaa = PT2257_level(dB);
	
	uint8_t aaaa = bbbaaaa & 0b00001111;
	uint8_t bbb = (bbbaaaa>>4) & 0b00001111;
	
	uint8_t data[2] = {		// Build WORD
		(EVC_L_10 | bbb),
		(EVC_L_1 | aaaa)
	};
	
	I2C_0_writeNBytes(PT2257_ADDR, data, 2);
	
}