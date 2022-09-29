#ifndef TCP_H
#define TCP_H
#include <kernel/types.h>

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
    uint16_t checksum;
    uint16_t urgentPtr;
    
} TcpHeader;

#endif