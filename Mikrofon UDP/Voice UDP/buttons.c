/*
 * buttons.c
 *
 * Created: 2019-01-11 21:18:21
 *  Author: Kajetan
 */ 
#include "buttons.h"

/*
		Deskryptor przycisku
*/
typedef struct{
	buttons_get_level_t get_level;	// wsk. do procedury zwracajacej stan przycisku, jesli null - przycisk  nieaktywny
	buttons_clb_t on_pressed;		// callback wywolywany gdy przycisnieto przycisk
	buttons_clb_t on_released;		// callback wywolywany gdy puszczono przycisk
	bool		  prev_state;		// poprzedni stan przycisku
	bool			pressed;		// Flagi oznaczajace ¿¹danie wywo³ania odpowiedniego callback'a 
	bool			released;	
}button_descriptor_t;

button_descriptor_t buttons[MAX_BUTTONS];


/*
		Rejestrowanie nowego przycisku
*/
void	buttons_register(Button btn, buttons_get_level_t get_level){
	if(btn < MAX_BUTTONS)
		buttons[btn].get_level = get_level;
}


/*
		Ustawianie callback'ow
*/
void	buttons_set_on_pressed_callback(Button btn, buttons_clb_t clb){
	if(btn < MAX_BUTTONS)
		buttons[btn].on_pressed = clb;
}
void	buttons_set_on_released_callback(Button btn, buttons_clb_t clb){
	if(btn < MAX_BUTTONS)
		buttons[btn].on_released = clb;	
}


/*
		Wlaczanie / wylaczanie czasomierza
*/
void RTC_ON(){
	while (RTC.STATUS > 0)	 /* Wait for all register to be synchronized */
		;
	RTC.CTRLA |= (1 << RTC_RTCEN_bp);     /* Enable: enabled */
}

void RTC_OFF(){
	while (RTC.STATUS > 0)	 /* Wait for all register to be synchronized */
	;
	RTC.CTRLA &= ~(1 << RTC_RTCEN_bp);     /* Enable: enabled */
}


/*
		Inicjalizacja przycisków
*/
void	buttons_init(){
	
	buttons_register(BTN1, BTN1_get_level);
	buttons_register(BTN2, BTN2_get_level);
	buttons_register(BTN3, BTN3_get_level);
	RTC_ON();
	
}


/*
		Przeniesienie obs³ugi callback'ów do pêtli g³ównej
*/
void	buttons_process(){
	
	uint8_t i, state;
	
	for (i=0; i<MAX_BUTTONS; i++){		// Sprawdzamy po kolei wszystkie przyciski
		
		if(buttons[i].on_pressed && buttons[i].pressed){		// Sprawdzamy czy trzeba wywo³aæ callback 'on_pressed'
			buttons[i].pressed = false;
			buttons[i].on_pressed();
		}
		
		if(buttons[i].on_released && buttons[i].released){		// Sprawdzamy czy trzeba wywo³aæ callback 'on_released'
			buttons[i].released = false;
			buttons[i].on_released();
		}
	}
}


/*
		Skanowanie przycisków ~20ms
*/
ISR(RTC_CNT_vect)
{

	/* Insert your RTC Overflow interrupt handling code */
	uint8_t i, state;
	
	for (i=0; i<MAX_BUTTONS; i++){		// Sprawdzamy po kolei wszystkie przyciski
		
		if(buttons[i].get_level){
			state = buttons[i].get_level();
		
			if(state != buttons[i].prev_state){	// zmiana stanu
				
				if(!state){		// Wcisnieto
					buttons[i].pressed = true;
				}else{			// Puszczono
					buttons[i].released = true;
				}		
				buttons[i].prev_state = state;			
			}
			
		}
	}
	
	/* Overflow interrupt flag has to be cleared manually */
	RTC.INTFLAGS = RTC_OVF_bm;
}
