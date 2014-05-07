#include <util/crc16.h>
#include <stdint.h>
#include <stddef.h>
#include "can_transport.h"
#include "hal.h"

header_s temp_tx_header;

uint16_t get_crc_of_buffer(msg_buffer_u *buffer, uint8_t length)
{
    uint16_t calc_crc_16 = 0x0000;
    for (uint8_t i =0; i < length - 2; i++) {
        calc_crc_16 = _crc_xmodem_update(calc_crc_16, buffer->buffer[i]);
    }
    return calc_crc_16;
}

void tx_msg_char(void)
{
    UDR0 = tx_buffer.buffer[tx_idx];
    tx_idx++;
    if (tx_idx == tx_len)
    {
        rx_state = RX_IDLE;
        tx_state = TX_IDLE;
        transport_state = RX;
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
    handler_fp handler;
    uint8_t error;
} type_switch_return_s;

type_switch_return_s type_switch(msg_buffer_u *buffer)
{
    type_switch_return_s app_msg_info;
    switch(buffer->msg.header.type)
    {
        case PING_REQ:
             app_msg_info.handler = ping_req_handler;
             app_msg_info.size = sizeof(PING_REQ_s);
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
    rx_buffer.buffer[rx_idx] = rx_char;
    rx_idx++;
}

void process_one_rx_char(uint8_t rx_char)
{
    switch (rx_state)
    {
        case RX_IDLE:
            if (rx_char == SYNC_BYTE_VALUE)
            {
                rx_idx = 0x00;
                rx_len = 0x00;
                rx_state = RX_HEADER;
                transport_state = RX;
                store_one_rx_char(rx_char);
            }
            break;
        case RX_HEADER:
            store_one_rx_char(rx_char);
            if (rx_idx == sizeof(rx_buffer.msg.header)) // done rxing header
            {
                //filter for address and version
                if (rx_buffer.msg.header.dest_addr == address.as_uint16 && rx_buffer.msg.header.version == VERSION)
                {
                    //demux type
                    type_switch_return_s app_msg_info = type_switch(&rx_buffer);
                    if (app_msg_info.error == 0)
                    {
                        rx_state = RX_MSG;
                        rx_len = CRC_SIZE + sizeof(rx_buffer.msg.header) + app_msg_info.size;
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
            store_one_rx_char(rx_char);
            if (rx_idx == rx_len) //done rxing message
            {
                
                //setup CRC
                bytes_and_uint16_u crc;
                crc.as_bytes.low = rx_buffer.buffer[rx_len - 2];
                crc.as_bytes.high = rx_buffer.buffer[rx_len - 1];
                //CRC passes
                if (crc.as_uint16 == get_crc_of_buffer(&rx_buffer, rx_len))
                {
                    if (rx_buffer.msg.header.type == ACK)
                    {
                        //ack received
                        rx_state = RX_IDLE;
                    }
                    else
                    {
                        rx_state = RX_SEND_ACK;
                        send_ack(rx_buffer.msg.header.src_addr);
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


void set_crc_of_tx_buffer(void)
{
    bytes_and_uint16_u crc;
    crc.as_uint16 = get_crc_of_buffer(&tx_buffer, tx_len);
    tx_buffer.buffer[tx_len - 2] = crc.as_bytes.low;
    tx_buffer.buffer[tx_len - 1] = crc.as_bytes.high;
}

void set_tx_header(uint16_t addr, uint16_t type)
{
    tx_buffer.msg.header.sync = SYNC_BYTE_VALUE;
    tx_buffer.msg.header.dest_addr = addr;
    tx_buffer.msg.header.src_addr = address.as_uint16;
    tx_buffer.msg.header.type = type;
    tx_buffer.msg.header.version = VERSION;
    type_switch_return_s app_msg_info = type_switch(&tx_buffer);
    tx_len = sizeof(tx_buffer.msg.header) + CRC_SIZE + app_msg_info.size;
}

void start_tx(void)
{
    tx_idx = 0x00;
    switch_to_tx();
    tx_msg_char();
}

void send_ack(uint16_t addr)
{
    temp_tx_header = tx_buffer.msg.header;
    set_tx_header(addr, ACK);
    set_crc_of_tx_buffer();
    start_tx();
}



void send_msg(uint16_t addr, uint16_t type)
{
    set_tx_header(addr, type);
    set_crc_of_tx_buffer();
    start_tx();
}

//for debug
void tx_char(char tx_char) {
    while ( !( UCSR0A & (1<<UDRE0)) );
    UDR0 = tx_char;
}



void set_address(uint16_t addr)
{
    address.as_uint16 = addr;
}

