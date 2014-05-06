#include <util/crc16.h>
#include <stdint.h>
#include <stddef.h>
#include "can_transport.h"
#include "hal.h"
//#include "cmn_msg_switch.h"


uint8_t msg_length = 0x00;

msg_buffer_t *buffer = &msg_buffer;

void tx_msg_char(void)
{
    UDR0 = buffer->raw_buffer[buffer_idx];
    buffer_idx++;
    if (buffer_idx == msg_length)
    {
        buffer = &msg_buffer;
        rx_state = RX_IDLE;
        switch_to_rx();
    }
}

void switch_to_tx(void)
{
    //turn on tx
    enable_tx();
    //turn on UDRE interrupt
    enable_udre_interrupt();
}

void switch_to_rx(void)
{
    // turn off UDRE interrupt
    disable_udre_interrupt();
    //turn on reciever
    enable_rx();
}



void ping_req_handler(void)
{
    //TODO reply
}
//end of autogen needed section
//

typedef struct type_switch_return_s
{
    size_t size;
    handler_fp_t handler;
    uint8_t error;
} type_switch_return_s;

type_switch_return_s type_switch()
{
    type_switch_return_s app_msg_info;
    switch(buffer->header.type)
    {
        case PING_REQ:
             app_msg_info.handler = ping_req_handler;
             app_msg_info.size = sizeof(PING_REQ_msg);
             app_msg_info.error = 0;
             break;
        default:
             app_msg_info.error = -1;
             break;
    }
    return app_msg_info;
}

void store_one_rx_char(uint8_t rx_char)
{
    PORTC ^=0x01;
    buffer->raw_buffer[buffer_idx] = rx_char;
    buffer_idx++;
}


void process_one_rx_char(uint8_t rx_char)
{
    switch (rx_state)
    {
        case RX_IDLE:
            if (rx_char == SYNC_BYTE_VALUE)
            {
                buffer = &msg_buffer;
                buffer_idx = 0x00;
                msg_length = 0x00;
                rx_state = RX_HEADER;
                transport_state = RX_ACTIVE;
                store_one_rx_char(rx_char);
            }
            break;
        case RX_HEADER:
            store_one_rx_char(rx_char);
            if (buffer_idx == sizeof(buffer->header)) // done rxing header
            {
                //filter for address and version
                if (buffer->header.dest_addr == address.address && buffer->header.version == VERSION)
                {
                    type_switch_return_s app_msg_info = type_switch();
                    //discover length from type
                    if (app_msg_info.error == 0)
                    {
                        rx_state = RX_MSG;
                        msg_length = CRC_SIZE + sizeof(buffer->header) + app_msg_info.size;
                    }
                    else
                    {
                        rx_state = RX_IDLE;
                    }
                }
                else
                {
                    rx_state = RX_IDLE;
                }
            }
            break;
        case RX_MSG:
            PORTC ^= 0x04;
            store_one_rx_char(rx_char);
            if (buffer_idx == msg_length) //done rxing message
            {
                crc_t crc;
                crc.crc_bytes.crc_low = buffer->raw_buffer[msg_length - 2];
                crc.crc_bytes.crc_high = buffer->raw_buffer[msg_length - 1];
                if (crc.crc == get_crc_of_buffer()) //CRC passes
                {
                    if (buffer->header.type == ACK_MSG_TYPE)
                    {
                        //ack received
                        rx_state = RX_IDLE;
                    }
                    else
                    {
                        PORTC ^= 0x02;
                        send_ack(buffer->header.src_addr);
                    }
                }
                else
                {
                    rx_state = RX_IDLE;
                }
            }
            break;
    }
}


void send_ack(uint16_t addr)
{
    buffer = &ack_buffer;
    buffer->header.sync = SYNC_BYTE_VALUE;
    buffer->header.dest_addr = addr;
    buffer->header.src_addr = address.address;
    buffer->header.type = ACK_MSG_TYPE;
    buffer->header.version = VERSION;
    msg_length = sizeof(buffer->header) + CRC_SIZE;
    crc_t crc;
    crc.crc = get_crc_of_buffer();
    buffer->raw_buffer[msg_length - 2] = crc.crc_bytes.crc_low;
    buffer->raw_buffer[msg_length - 1] = crc.crc_bytes.crc_high;
    buffer_idx = 0x00;
    switch_to_tx();
    tx_msg_char();
}

void send_msg(uint16_t addr, uint16_t type)
{
    enable_tx();
    transport_state = TX_ACTIVE;
    buffer = &msg_buffer;
    buffer->header.sync = SYNC_BYTE_VALUE;
    buffer->header.type = type;
    buffer->header.dest_addr = addr;
    buffer->header.src_addr = address.address;
    buffer->header.version = VERSION;
    type_switch_return_s app_msg_info = type_switch();
    msg_length = CRC_SIZE + sizeof(buffer->header) + app_msg_info.size;
    crc_t crc;
    crc.crc = get_crc_of_buffer();
    buffer->raw_buffer[msg_length - 2] = crc.crc_bytes.crc_low;
    buffer->raw_buffer[msg_length - 1] = crc.crc_bytes.crc_high;
    buffer_idx = 0x00;
    switch_to_tx();
    tx_msg_char();
}

//for debug
void tx_char(char tx_char) {
    while ( !( UCSR0A & (1<<UDRE0)) );
    UDR0 = tx_char;
}


uint16_t get_crc_of_buffer(void)
{
    uint16_t calc_crc_16 = 0x0000;
    uint8_t i = 0;
    for (i =0; i < msg_length - 2; i++) {
        calc_crc_16 = _crc_xmodem_update(calc_crc_16, buffer->raw_buffer[i]);
    }
    return calc_crc_16;

}

void set_address(uint16_t addr)
{
    address.address = addr;
}

