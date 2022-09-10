#if !defined(RTL8139)
#define RTL8139
#include <kernel/drivers/pci.h>
#include <kernel/types.h>
struct packet_header
{
    union
    {
        uint16_t bheader;
        struct
        {
            // Recieve Ok;
            uint16_t rok :1;
            // Frame Alignment Error
            uint16_t fae :1;
            // CRC Error
            uint16_t crc :1;
            // Long Packet
            uint16_t lng :1;
            // Runt Packet
            uint16_t runt :1;
            // Invalid Symbol Error
            uint16_t ise :1;
            uint16_t _ :7;
            // Broadcast Address Received
            uint16_t bar :1;
            // Physical Address Matched
            uint16_t pam :1;
            //Multicast address received
            uint16_t mar :1;
        };
    };

    uint16_t data_size;
} __attribute__((packed));

void rtl8139_install(pci_config_t * _device);
void read_mac_addr(uint8_t mac_addr[]);
void rtl8139_send_packet(ptr_t data,uint32_t len);

#endif // RTL8139
