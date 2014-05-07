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

typedef void (*handler_fp)(void);

typedef enum
{
    RX_IDLE,
    RX_HEADER,
    RX_MSG,
    RX_SEND_ACK,
} rx_state_enum;

typedef enum 
{
    RX,
    TX,
} transport_state_enum;

typedef enum 
{
    TX_IDLE,
    TX_WAITING_FOR_RX,
    TX_CARRIER_SENSE,
    TX_SEND,
    TX_COLLISION,
    TX_ACK,
} tx_state_enum;

uint8_t tx_retry_count;
uint8_t tx_collision_count;

//move this section to an autogen library
typedef enum
{
    ACK = 0xFFFF,
    PING_REQ = 0xEEEE,
} msgs_enum;

typedef struct PING_REQ_s
{
    uint8_t tag;
} PING_REQ_s;

//end autogen section

typedef union app_msgs_u
{
    PING_REQ_s ping_req;
} app_msgs_u;


typedef struct header_s 
{
    uint8_t sync;
    uint8_t priority;
    uint16_t type;
    uint16_t dest_addr;
    uint16_t src_addr;
    uint8_t version;
} header_s;

typedef struct header_and_app_msgs_s
{
    header_s header;
    app_msgs_u app_msgs;
} header_and_app_msgs_s;

typedef union msg_buffer_u {
    uint8_t buffer[sizeof(header_and_app_msgs_s)];
    header_and_app_msgs_s msg;
} msg_buffer_u;

msg_buffer_u rx_buffer;
msg_buffer_u tx_buffer;

uint8_t rx_idx;
uint8_t tx_idx;

uint8_t rx_len;
uint8_t tx_len;

typedef struct two_bytes_s 
{
    uint8_t low;
    uint8_t high;
} two_bytes_s;

typedef union bytes_and_uint16_u
{
    two_bytes_s as_bytes;
    uint16_t as_uint16;
} bytes_and_uint16_u;


bytes_and_uint16_u address;

uint8_t transport_state;
uint8_t tx_state;
uint8_t rx_state;


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
