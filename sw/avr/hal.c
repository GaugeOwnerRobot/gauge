#include <avr/io.h>
#include "hal.h"


void setup_timer0(void) {
    TCCR0B |= (1 << CS02) | (0 << CS01) | (0 << CS00);
    TIMSK0 |= (1 <<  TOIE0);
}

void disable_udre_interrupt(void) {
    UCSR0B &= ~(1 << UDRIE0 );
}

void enable_udre_interrupt(void) {
    UCSR0B |= (1 << UDRIE0 );
}

void enable_rx(void) {
    //disable tx
    UCSR0B &= ~(1 << TXEN0);
    //enable rx
    UCSR0B |= (1 << RXEN0);
}

void enable_tx(void) {
    //disable rx
    UCSR0B &= ~(1 << RXEN0);
    //enable tx
    UCSR0B |= (1 << TXEN0);
}

void init_collision_detect_pins(void) {
    DDRD |= (1 << CAN_SILENT); //CAN silence pin as output
    DDRD &= ~(1 << COLLISION_DETECT); //collision detect pin as input
    PORTD |= (1 << CAN_SILENT); //mute CAN;
}

void init_sync_pins(void) {
    DDRD &= ~(1 << SYNC0); //collision detect pin as input
    DDRD &= ~(1 << SYNC1); //collision detect pin as input
}

void mute_can_xceiver(void) {
    PORTD |= (1 << CAN_SILENT); //mute CAN;
}

void unmute_can_xceiver(void) {
    PORTD &= ~(1 << CAN_SILENT); //unmute CAN
}

//set up initial values for uart rx mode
void uart_init(void){
    UBRR0H = (BAUD_UBRR >> 8) & 0xFF;
    UBRR0L = BAUD_UBRR & 0xFF; //set baud rate
    UCSR0A = (1 << U2X0); //double speed, all other options off

    UCSR0B = ((1 << RXCIE0) | //rx interrupt, always on
            (0 << TXCIE0) | //tx interrupt, always off, udrie used instead
            (0 << UDRIE0) | //data register empty interrupt, used to continue transmission
            (1 << RXEN0) | //rx enable, start in rx mode
            (0 << TXEN0) | //tx disable
            (0 << UCSZ02)); // 9 data bits, disabled

    UCSR0C = ( // no parity
            //one stop bit USBS0 ==0
            (1 << UCSZ01) |
            (1 << UCSZ00)); //8 data bits, enabled
}
