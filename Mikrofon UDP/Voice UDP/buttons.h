/*
 * buttons.h
 *
 * Created: 2019-01-11 21:18:35
 *  Author: Kajetan
 */ 


#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <driver_init.h>
#include <compiler.h>

#define MAX_BUTTONS 3

// Identyfikatory przycisków
#define BTN1 0
#define BTN2 1
#define BTN3 2


// defincje typow
typedef void (*buttons_clb_t)(void);
typedef bool (*buttons_get_level_t)(void);

typedef uint8_t Button;


void	buttons_init();
void	buttons_process();
void	buttons_set_on_pressed_callback(Button btn, buttons_clb_t clb);
void	buttons_set_on_released_callback(Button btn, buttons_clb_t clb);

#endif /* BUTTONS_H_ */