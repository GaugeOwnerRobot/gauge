#ifndef HAL_H
#define HAL_H

#include <avr/io.h>
#define CAN_SILENT PD4
#define COLLISION_DETECT PD2
#define SYNC0 PD6
#define SYNC1 PD7

//baud rate
#define BAUD_UBRR 64 // 38400 in double speed mode

void disable_udre_interrupt(void);
void enable_udre_interrupt(void);
void enable_rx(void);
void enable_tx(void);
void init_collision_detect_pins(void);
void init_sync_pins(void);
void mute_can_xceiver(void);
void unmute_can_xceiver(void);
void uart_init(void);
void setup_timer0(void);
#endif //HAL_H
