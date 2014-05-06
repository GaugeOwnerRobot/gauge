#ifndef CAN_TRANSPORT_H
#define CAN_TRANSPORT_H
#include <stdint.h>

#define BUFFER_SIZE 64
#define MSG_OVERHEAD 10
#define ADDR_SIZE 2
#define LENGTH_POSITION 5 
#define MSG_TYPE_SIZE 2
#define VERSION_SIZE 1
#define CRC_SIZE 2
#define SYNC_BYTE_VALUE 0x55


//temp
#define VERSION 0

typedef void (*handler_fp_t)(void);

typedef enum
{
    RX_IDLE,
    RX_HEADER,
    RX_MSG,
} rx_state_enum;

typedef enum 
{
    IDLE,
    TX_ACTIVE,
    RX_ACTIVE,
    COLLISION_BACKOFF,
    WAITING_FOR_ACK,
    APP_BUFFER_LOCK,
} transport_state_enum;

typedef enum 
{
    ACK_IDLE,
} ack_state_enum;

typedef struct can_header_t {
    uint8_t sync;
    uint16_t type;
    uint16_t dest_addr;
    uint16_t src_addr;
    uint8_t version;
} can_header_t;

typedef union msg_buffer_t {
    uint8_t raw_buffer[BUFFER_SIZE];
    can_header_t header;
} msg_buffer_t;

msg_buffer_t msg_buffer;

typedef union ack_buffer_t
{
    uint8_t raw_buffer[sizeof(can_header_t) + CRC_SIZE];
    can_header_t header;
} ack_buffer_t;

ack_buffer_t ack_buffer;

typedef struct address_bytes_t {
    uint8_t addr_low;
    uint8_t addr_high;
} address_bytes_t;

typedef union address_t {
    address_bytes_t address_bytes;
    uint16_t address;
} address_t;

typedef struct crc_bytes_t {
    uint8_t crc_low;
    uint8_t crc_high;
} crc_bytes_t;

typedef union crc_t {
    crc_bytes_t crc_bytes;
    uint16_t crc;
} crc_t;


//move this section to an autogen library
typedef enum
{
    ACK_MSG_TYPE = 0x0000,
    PING_REQ = 0xEEEE,
} messages;

typedef struct PING_REQ_msg
{
    uint8_t tag;
} PING_REQ_msg;

uint8_t transport_state;
uint8_t ack_state;
uint8_t rx_state;

address_t address;
uint8_t buffer_idx;

uint16_t get_crc_of_buffer(void);
void tx_char(char tx_char);
void process_one_rx_char(uint8_t rx_char);
void store_one_rx_char(uint8_t rx_char);
void switch_to_rx(void);
void switch_to_tx(void);
void tx_msg_char(void);
void send_ack(uint16_t addr);
void send_msg(uint16_t addr, uint16_t type);
void set_address(uint16_t addr);

#endif //CAN_TRANSPORT_H
