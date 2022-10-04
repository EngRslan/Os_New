#ifndef TCP_H
#define TCP_H
#include <kernel/types.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>
#include <kernel/datastruct/link.h>
#include <stdbool.h>

#define TCP_FIN     (1 << 0)
#define TCP_SYN     (1 << 1)
#define TCP_RST     (1 << 2)
#define TCP_PSH     (1 << 3)
#define TCP_ACK     (1 << 4)
#define TCP_URG     (1 << 5)

#define SEQ_LT(x,y) ((int)((x)-(y)) < 0)
#define SEQ_LE(x,y) ((int)((x)-(y)) <= 0)
#define SEQ_GT(x,y) ((int)((x)-(y)) > 0)
#define SEQ_GE(x,y) ((int)((x)-(y)) >= 0)


typedef struct TcpChecksum{
    Ipv4Address src;
    Ipv4Address dst;
    uint8_t res;
    uint8_t protocol;
    uint16_t len;

} __attribute((packed)) TcpChecksum;
typedef union TcpFlags
{
    uint8_t bits;
    struct
    {
        
        uint8_t isFinish    :1;
        uint8_t isSync      :1;
        uint8_t isReset     :1;
        uint8_t isPush      :1;
        uint8_t isAck       :1;
        uint8_t isUrgent    :1;
        uint8_t _           :2;
    };
} TcpFlags;

typedef struct TcpHeader
{
    uint16_t srcPort;
    uint16_t dstPort;
    uint32_t seqNum;
    uint32_t ackNum;
    uint8_t dataOffset ;
    TcpFlags flags;
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
    Link link;
    TcpState state;
    Ipv4Address localAddr;
    Ipv4Address remoteAddr;
    NetPort localPort;
    NetPort remotePort;
    NetInterface *intf;
    
    // System Initial Sequnce Number
    uint32_t iss;
    // System Sequence
    uint32_t nxtSeq;

    // Initaial Receive Number
    uint32_t riss;
    uint32_t rNxtSeq;

    // Send State (MYOS)
    uint32_t unAck;    // Send Unacknowledge pointer
    // uint32_t sndNxt;    // send next
    // uint32_t iss;       // initial send sequence number


    // uint32_t sndWnd;    // Send window
    // uint32_t sndUP;     // send urgent pointer
    // uint32_t sndWl1;    // segment sequence number
    // uint32_t sndWl2;    // segment ack number

    // Receive State (Other Partner)
    // uint32_t rcvNxt;
    // uint32_t irs;   //initial receive sequence number

    // uint32_t rcvWnd;
    // uint32_t rcvUP;

    // uint32_t mslWait;


    void (*onError)(struct TcpConnection *conn, uint32_t error);
    void (*onState)(struct TcpConnection *conn, TcpState oldState, TcpState newState);
    void (*onData)(struct TcpConnection *conn, const uint8_t *data, uint32_t len);
} TcpConnection;


TcpConnection *TcpCreate();
void TcpInit();
bool TcpConnect(TcpConnection *conn, Ipv4Address addr,uint16_t port);
void TcpClose(TcpConnection *conn);
void TcpSend(TcpConnection *conn, const void *data, uint32_t count);
#endif