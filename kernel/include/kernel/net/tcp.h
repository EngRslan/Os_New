#ifndef TCP_H
#define TCP_H
#include <kernel/types.h>
#include <kernel/net/addr.h>
#include <stdbool.h>
typedef struct Link
{
    Link *next;
    Link *prev;
}Link;

typedef struct TcpHeader
{
    uint16_t srcPort;
    uint16_t dstPort;
    uint32_t seqNum;
    uint32_t ackNum;
    union
    {
        uint16_t flagsbits;
        struct
        {
            uint8_t dataOffset  :4;
            uint8_t _           :6;
            uint8_t isUrgent    :1;
            uint8_t isAck       :1;
            uint8_t isPush      :1;
            uint8_t isReset     :1;
            uint8_t isSync      :1;
            uint8_t isFinish    :1;
        };
        
    };
    uint16_t windowSize;
    uint16_t checksum;
    uint16_t urgentPtr;
    
}__attribute__((packed)) TcpHeader;

typedef enum TcpState {
    TCP_CLOSED      = 0,
    TCP_LISTEN      = 1,
    TCP_SYN_SENT    = 2,
    TCP_SYN_RECEIVED= 3,
    TCP_ESTABLISHED = 4,
    TCP_FIN_WAIT_1  = 5,
    TCP_FIN_WAIT_2  = 6,
    TCP_CLOSE_WAIT  = 7,
    TCP_CLOSING     = 8,
    TCP_LAST_ACK    = 9,
    TCP_TIME_WAIT   = 10,
}TcpState;

typedef struct TcpConnection
{
    TcpState state;
    Ipv4Address localAddr;
    Ipv4Address remoteAddr;
    uint16_t localPort;
    uint16_t remotePort;

    // Send State
    uint32_t sndUna;    // Send Unacknowledge pointer
    uint32_t sndNxt;    // send next
    uint32_t sndWnd;    // Send window
    uint32_t sndUP;     // send urgent pointer
    uint32_t sndWl1;    // segment sequence number
    uint32_t sndWl2;    // segment ack number
    uint32_t iss;       // initial send sequence number

    // Receive State
    uint32_t rcvNxt;
    uint32_t rcvWnd;
    uint32_t rcvUP;
    uint32_t irs;   //initial receive sequence number

    uint32_t mslWait;

    void (*onError)(struct TcpConnection *conn, uint32_t error);
    void (*onState)(struct TcpConnection *conn, TcpState oldState, TcpState newState);
    void (*onData)(struct TcpConnection *conn, const uint8_t *data, uint32_t len);
} TcpConnection;


TcpConnection *TcpCreate();
bool TcpConnect(TcpConnection *conn, const Ipv4Address *addr,uint16_t port);
void TcpClose(TcpConnection *conn);
void TcpSend(TcpConnection *conn, const void *data, uint32_t count);
#endif