//#define F_CPU 20000000UL
//#include <util/delay.h>

#define BAUD_UBRR 64 // 38400 in double speed mode
#include <avr/io.h>
#include <avr/interrupt.h>
#include "can_transport.h"
#include "hal.h"
//#include "gpio_lib.h"
//#include "isr.h"

//#include <stdio.h>

//static int uart_putchar(char c, FILE *stream);
//static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
//static int uart_putchar(char c, FILE *stream) {
//    while ( !( UCSR0A & (1<<UDRE0)) );
//    UDR0 = c;
//    return 0;
//}


ISR(TIMER0_OVF_vect){
    //TCNT0 = 0xFF;
    //PORTC ^= 0xFF;
    //switch(timer0_task) {
    //    case WAIT_BEFORE_TX:
    //        //start_tx();
    //        break;
    //}
}


//ISR(USART_TX_vect){
//
//}

//uart data register empty
ISR(USART_UDRE_vect){
    tx_msg_char();
}

//char rx
ISR(USART_RX_vect){
    uint8_t rx_char = UDR0;
    process_one_rx_char(rx_char);
}


int main(void)
{
    DDRC = 0xFF;
    uart_init();
//    stdout = &mystdout;
    //setup_timer0();
    set_address(0xFFFF);

    sei();

    while(PIND & (1 <<7));
    tx_buffer.msg.app_msgs.ping_req.tag = 0x11;
    send_msg(0x1234, PING_REQ, 0x00);
    while (1)
    {
    }

        //PORTC = 0xFF;
        //PORTC = 0x00;
        //tx_char(0x55);

    return 1;
}

