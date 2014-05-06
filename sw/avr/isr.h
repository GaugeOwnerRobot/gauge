#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>
#include "hal.h"
#include "can_transport.h"
ISR(TIMER0_OVF_vect);


ISR(INT0_vect);


ISR(USART_TX_vect);

//uart data register empty
ISR(USART_UDRE_vect);

//char rx
ISR(USART_RX_vect);

