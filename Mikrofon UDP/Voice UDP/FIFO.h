/*
 * FIFO.h
 *
 * Created: 2018-10-15 22:37:16
 *  Author: Kajetan
 */ 


#ifndef FIFO_H_
#define FIFO_H_

#include <atmel_start.h>
#include "VoiceProcessing.h"

#define FIFO_SIZE 4

typedef struct{			// Struktura kolejki FIFO z mozliwoscia przelaczania buforow
	
	uint8_t buffer[FIFO_SIZE][VOICE_PACKET_SIZE];	// 2D FIFO buffer
	uint8_t packet_buf;								// packet operations buffer ptr
	uint8_t sample_buf;								// sample operations buffer ptr
	uint8_t ptr;									// FIFO internal pointer r/w
	
	uint8_t packet_count;							// ilosc dostepnych pakietow (do wyslania / odczytania) (SEND)
	uint8_t id;										// identyfikator pakietu (0 - 255)						(SEND)
	
}FIFO_t;

extern FIFO_t R_FIFO;		// 2 instances of FIFO for receiving & sending packets
extern FIFO_t S_FIFO;

/*
			Odczytywanie buforow dla zapisu / odczytu calego pakietu
*/
inline uint8_t* get_receive_packet_buf(){	// Zapisujemy pakiet do receive fifo
	return &( R_FIFO.buffer[R_FIFO.packet_buf][0] );
}

inline uint8_t* get_send_packet_buf(){		// Odczytujemy pakiet z send fifo
	return &( S_FIFO.buffer[S_FIFO.packet_buf][0] );
}

void switch_receive_packet_buf();
void switch_send_packet_buf();
void write_sample(uint8_t *sample);
void read_sample(uint8_t *sample);

#endif /* FIFO_H_ */