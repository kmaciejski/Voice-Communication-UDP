/*
 * FIFO.c
 *
 * Created: 2018-10-15 22:36:12
 *  Author: Kajetan
 */ 

#include "FIFO.h"
#include <atomic.h>


FIFO_t R_FIFO = {0};		// 2 instances of FIFO for receiving & sending packets
FIFO_t S_FIFO = {0};


/*
			Przelaczanie wsteczne buforow dla calych pakietow
*/
void switch_back_receive_packet_buf(){
	if(R_FIFO.packet_buf){
		R_FIFO.packet_buf--;
	}else{
		R_FIFO.packet_buf = (FIFO_SIZE - 1);
	}		
}

void switch_back_send_packet_buf(){
	if(S_FIFO.packet_buf){
		S_FIFO.packet_buf--;
	}else{
		S_FIFO.packet_buf = (FIFO_SIZE - 1);
	}	
}


/*
			Przelaczanie buforow dla calych pakietow
*/
void switch_receive_packet_buf(){
	// First get ID
	R_FIFO.id = * get_receive_packet_buf();
	
	R_FIFO.packet_buf++;
	
	if(R_FIFO.packet_buf >= FIFO_SIZE)
		R_FIFO.packet_buf = 0;
		
	if(R_FIFO.packet_buf == R_FIFO.sample_buf)	// Wskazniki zapisu i odczytu nie mog¹ siê spotkaæ !
		switch_back_receive_packet_buf();				// rezygnujemy z przelaczenia bufora
	else
		R_FIFO.packet_count++;

}

void switch_send_packet_buf(){
	if(S_FIFO.packet_count){	// jesli sa dostepne pakiety
		S_FIFO.packet_buf++;
	
		if(S_FIFO.packet_buf >= FIFO_SIZE)
			S_FIFO.packet_buf = 0;
		
		if(S_FIFO.packet_buf == S_FIFO.sample_buf)	// Wskazniki zapisu i odczytu nie mog¹ siê spotkaæ !
			switch_back_send_packet_buf();				// rezygnujemy z przelaczenia bufora		

		S_FIFO.packet_count--;	// zmniejszamy ilosc pakietow o 1
	}
}


/*
			Przelaczanie wsteczne buforow probek
*/
void switch_back_receive_sample_buf(){
	if(R_FIFO.sample_buf){
		R_FIFO.sample_buf--;
	}else{
		R_FIFO.sample_buf = (FIFO_SIZE - 1);
	}		
}

void switch_back_send_sample_buf(){
	if(S_FIFO.sample_buf){
		S_FIFO.sample_buf--;
	}else{
		S_FIFO.sample_buf = (FIFO_SIZE - 1);
	}	
}


/*
			Przelaczanie buforow probek
*/
void switch_receive_sample_buf(){
	R_FIFO.sample_buf++;
	
	if(R_FIFO.sample_buf >= FIFO_SIZE)
		R_FIFO.sample_buf = 0;
		
	if(R_FIFO.sample_buf == R_FIFO.packet_buf)	// Wskazniki zapisu i odczytu nie mog¹ siê spotkaæ !
		switch_back_receive_sample_buf();				// rezygnujemy z przelaczenia bufora
	else
		R_FIFO.packet_count--;
}

void switch_send_sample_buf(){
	S_FIFO.sample_buf++;
	
	if(S_FIFO.sample_buf >= FIFO_SIZE)
		S_FIFO.sample_buf = 0;
		
	if(S_FIFO.sample_buf == S_FIFO.packet_buf)	// Wskazniki zapisu i odczytu nie mog¹ siê spotkaæ !
		switch_back_send_sample_buf();				// rezygnujemy z przelaczenia bufora		
	else
		S_FIFO.packet_count++;
}


/*
			zapis / odczyt probek
*/
void write_sample(uint8_t *sample){
	
	S_FIFO.ptr++;	// nie moze byc nigdy 0
	
	S_FIFO.buffer[S_FIFO.sample_buf][S_FIFO.ptr] = *sample;	
	
	if(S_FIFO.ptr >= VOICE_DATA_SIZE){		
		// Pakiet zapelniony, ustawiamy wskaznik probek na poczatek nowego pakietu
		S_FIFO.ptr = 0;
		
		// Dokladamy i zwiekszamy ID
		S_FIFO.buffer[S_FIFO.sample_buf][0] = S_FIFO.id++;
		
		// Przelaczamy bufor
		switch_send_sample_buf();
		
	}
}

void read_sample(uint8_t *sample){
	R_FIFO.ptr++;	// nie moze byc nigdy 0
	
	*sample = R_FIFO.buffer[R_FIFO.sample_buf][R_FIFO.ptr];
	
	if(R_FIFO.ptr >= VOICE_DATA_SIZE){
		// Pakiet zapelniony, ustawiamy wskaznik probek na poczatek nowego pakietu
		R_FIFO.ptr = 0;
		
		// Przelaczamy bufor
		switch_receive_sample_buf();
	}	
}