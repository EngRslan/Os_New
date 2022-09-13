#include <kernel/drivers/pci.h>
#include <kernel/types.h>
#include <kernel/drivers/rtl8139.h>
#include <kernel/system.h>
#include <kernel/bits.h>
#include <kernel/mem/vmm.h>
#include <kernel/mem/kheap.h>
#include <kernel/isr.h>
#include <logger.h>
#include <stddef.h>
#include <string.h>
#include <kernel/system.h>
#include <kernel/net/ethernet.h>
#include <kernel/net/intf.h>

#define ETH_BUFFER_ADDRESS GET_VIRTUAL_ADDRESS(0x302,0)

NetInterface *_netIf;
/*Virtual Buffer Address*/
ptr_t const rx_buffer   = (ptr_t)ETH_BUFFER_ADDRESS;
ptr_t const tx_buffer1  = (ptr_t)(rx_buffer + 8192 + 16 + 1500);
ptr_t const tx_buffer2  = (ptr_t)(tx_buffer1 + 645) ;
ptr_t const tx_buffer3  = (ptr_t)(tx_buffer2 + 645) ;
ptr_t const tx_buffer4  = (ptr_t)(tx_buffer3 + 645) ;
uint32_t rx_offset = 0;

uint32_t ts_array[4][3]={
    {0x20,0x10,(uint32_t)tx_buffer1},
    {0x24,0x14,(uint32_t)tx_buffer2},
    {0x28,0x18,(uint32_t)tx_buffer3},
    {0x2C,0x1C,(uint32_t)tx_buffer4},
};

uint8_t current_tx = 0;

uint32_t io_addr = 0;
uint8_t get_hwverid(uint32_t io_addr,string_t * s){
    uint32_t hwverid = ((inportl(io_addr + 0x40))&0x7E000000)>>25;
    if(s){
        const char * vers[]={ "RTL8139", "RTL8139A", "RTL8139A-G", "RTL8139B", "RTL8130", "RTL8139C", "Unknown" };
        switch (hwverid)
        {
            case 0x30://0b00110000:
                *s = (string_t)vers[0];
                break;
            case 0x38://0b00111000:
                *s = (string_t)vers[1];
                break;
            case 0x39://0b00111001:
                *s = (string_t)vers[2];
                break;
            case 0x3C://0b00111100:
                *s = (string_t)vers[3];
                break;
            case 0x3E://0b00111110:
                *s = (string_t)vers[4];
                break;
            case 0x3A://0b00111010:
                *s = (string_t)vers[5];
                break;
        default:
            *s = (string_t)vers[6];
            break;
        }
    }
    return hwverid;
}
void read_mac_addr(uint8_t mac_addr[]){
    mac_addr[0] = inportb(io_addr + 0);
    mac_addr[1] = inportb(io_addr + 1);
    mac_addr[2] = inportb(io_addr + 2);
    mac_addr[3] = inportb(io_addr + 3);
    mac_addr[4] = inportb(io_addr + 4);
    mac_addr[5] = inportb(io_addr + 5);
}
void reset_device(uint32_t io_addr){
    outportb(io_addr+0x37,0x10);
    while (BITREAD(inportb(io_addr+0x37),4) != 0) { }
}
void rtl8139_send_packet(ptr_t data,uint32_t len){
    if(len > 645){
        log_warning("packet size %d exceeded buffer size 645 packet trancated",len);
        len = 645;
    }
    void * tx_buffer = (void *)ts_array[current_tx][2];
    memcpy(tx_buffer,(void *)data,len);

    outportl(io_addr + ts_array[current_tx][0],virtual2physical((v_addr_t)ts_array[current_tx][2]));
    outportl(io_addr + ts_array[current_tx][1],len);
    
    if(++current_tx>3)
        current_tx = 0;
    
    
}
uint8_t rx_packet_valid(struct packet_header * packet){
    uint8_t isbad = packet->runt || packet->lng || packet->crc || packet->fae;
    return !isbad && packet->rok;
}
void packet_recieved_handler(){
    /*
        Ethernet Packet
        2byte packet header
        2byte packet length
    */
   uint8_t cmd;
   while (1)
   {
        cmd = inportb(io_addr + 0x37);
        if(BITREAD(cmd,0)) // first bit Buffer empty
        {
            break;
        }

        do
        {
            struct packet_header * pct = (struct packet_header *)(rx_buffer + rx_offset) ;
            if(rx_packet_valid(pct)){
                NetBuffer *packet_buffer = (NetBuffer *)kmalloc(sizeof(NetBuffer));
                packet_buffer->packetData = kmalloc(pct->data_size);
                packet_buffer->interface = _netIf;
                packet_buffer->length = pct->data_size;
                memcpy(packet_buffer->packetData,(void *)pct+sizeof(struct packet_header),packet_buffer->length);
                // ethernet_handle_packet((struct ether_header *)packet_data,pct->data_size);
                EthernetReceive(packet_buffer);
                kfree(packet_buffer->packetData);
                kfree(packet_buffer);
                rx_offset = (rx_offset+pct->data_size+4+3)&(~0x3);

                if(rx_offset > 0x2000){
                    rx_offset -= 0x2000;
                }
                uint32_t ss = rx_offset - 0x10;
                outports(io_addr+0x38,rx_offset-0x10);
            }else{
                break;
            }
            cmd = inportb(io_addr + 0x37);
        } while (!BITREAD(cmd,0));
   }
}
void eth_irq_handler(register_t * reg){
    uint16_t status = inports(io_addr + 0x3e);

    if(status & (1<<2)){
        log_trace("eth: IRQ Fired (Packet Sent)");
    }

    if(status & (1<<0)){
        log_trace("eth: IRQ Fired (Packet Received)");
        packet_recieved_handler();
    }

    outports(io_addr + 0x3E,0x5);
}
void rtl8139Send(NetBuffer *packet_buffer){
    rtl8139_send_packet(packet_buffer->packetData,packet_buffer->length);
}
void rtl8139_install(NetInterface *netIf, pci_config_t * _device){
    pci_device_config_t * dev = (pci_device_config_t *)_device->config;
    io_addr = dev->BAR0;
    io_addr = io_addr & (~0x3);

    outportb(io_addr+0x52,0);
    reset_device(io_addr);

    // uint8_t mac_addr[6];

    string_t hwvers = NULL;
    uint32_t hwverid = get_hwverid(io_addr,&hwvers);
    memcpy(&netIf->name,hwvers,strlen(hwvers));

    char mac_str[19];
    read_mac_addr((uint8_t *)&netIf->macAddress);
    MacToStr(mac_str,netIf->macAddress);
    log_trace("[rtl8139]: %s Detected MAC:%s",hwvers,mac_str);

    callocate_region(kernel_directory,(v_addr_t)rx_buffer,3,0,1);
    memset((void *)rx_buffer,0,3*PAGE_SIZE);
    
    uint16_t pci_command = dev->common.command;
    pci_command |= (1<<2);
    pci_writew(_device->bus,_device->device,_device->function,PCI_COMMAND,pci_command);

    ptr_t rx_ph_buffer = (ptr_t)virtual2physical((v_addr_t)rx_buffer);
    outportl(io_addr + 0x30,(uint32_t)rx_ph_buffer);

    outports(io_addr + 0x3c,0x0005);
    outportl(io_addr + 0x44,0xf|(1<<7));
    outportb(io_addr + 0x37,0x0c);

    uint8_t irq = 32 + dev->interrupt_line;
    register_interrupt_handler(irq,eth_irq_handler);
    netIf->send = rtl8139Send;
}