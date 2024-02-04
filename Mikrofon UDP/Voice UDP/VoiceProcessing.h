/*
 * VoiceProcessing.h
 *
 * Created: 2018-11-08 00:19:48
 *  Author: Kajetan
 */ 


#ifndef VOICEPROCESSING_H_
#define VOICEPROCESSING_H_
#include <atmel_start.h>

#define VOICE_DATA_SIZE			64										// (align to 16 bytes !)
#define VOICE_ID_SIZE			1
#define VOICE_PACKET_SIZE		(VOICE_DATA_SIZE + VOICE_ID_SIZE)		// data + ID
#define VOICE_ID_OFFSET			0
#define VOICE_DATA_OFFSET		(VOICE_ID_SIZE + VOICE_ID_OFFSET)

/*
				Voice procedures & threads
*/
void	voice_init();
void	voice_process();

void	updateEncryptionKeyEx();		// Aktualizuje klucz rozszerzony (dla szybkiego szyfrowania)
void	voice_port_init();

/*
		RTC start / stop procedures
*/
inline void TC0_start(){
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

inline void TC0_stop(){
	TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
}
	
#endif /* VOICEPROCESSING_H_ */