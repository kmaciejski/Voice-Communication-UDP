/*
 * PT2257.h
 *
 * Created: 2018-11-03 18:05:15
 *  Author: Kajetan
 */ 


#ifndef PT2257_H_
#define PT2257_H_


#define PT2257_ADDR 0x44        //Chip address
#define EVC_OFF     0b11111111  //Function OFF (-79dB)
#define EVC_2CH_1   0b11010000  //2-Channel, -1dB/step
#define EVC_2CH_10  0b11100000  //2-Channel, -10dB/step
#define EVC_L_1     0b10100000  //Left Channel, -1dB/step
#define EVC_L_10    0b10110000  //Left Channel, -10dB/step
#define EVC_R_1     0b00100000  //Right Channel, -1dB/step
#define EVC_R_10    0b00110000  //Right Channel, -10dB/step
#define EVC_MUTE	0b01111000	//2-Channel MUTE

void PT2257_setLAttenuation(uint8_t dB);
void PT2257_setRAttenuation(uint8_t dB);
void PT2257_init();

#endif /* PT2257_H_ */