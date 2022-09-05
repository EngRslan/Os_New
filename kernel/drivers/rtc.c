#include <kernel/drivers/rtc.h>
#include <kernel/system.h>
#include <kernel/isr.h>
#include <kernel/types.h>
#include <kernel/bits.h>

#define BIN_ALIGN(x)((x & 0xF0)>>1)+ ((x & 0xF0)>>3) + (x & 0xf)

struct tm _tm;
uint8_t is_24h = 0; // 0=12h, 1=24h;
uint8_t is_bin = 0; // 0=bcd, 1=Binary; 

uint8_t read_cmos_register(uint8_t reg_num){
    outportb(PORT_REGISTER,reg_num | 0x80);
    //TODO SOME Delay
    return inportb(PORT_DATA);
}
void write_cmos_register(uint8_t reg_num,uint8_t data){
    outportb(PORT_REGISTER,reg_num | 0x80);
    //TODO SOME Delay
    outportb(PORT_DATA, data);
}
uint8_t bin_align(uint8_t num){
    if(is_bin != 1){
        return BIN_ALIGN(num);
    }
    return num;
}
uint8_t format_align(uint8_t num){
    if(is_24h != 1){
        return BIN_ALIGN(num);
    }
    return is_24h;
}
void handle_rtc_inturrept(register_t * reg){
    _tm.tm_sec  = bin_align(read_cmos_register(CMOS_REG_SECONDS));
    _tm.tm_min  = bin_align(read_cmos_register(CMOS_REG_MINUTES));
    _tm.tm_hour = bin_align(read_cmos_register(CMOS_REG_HOURS));
    _tm.tm_mday = bin_align(read_cmos_register(CMOS_REG_MDAY));
    _tm.tm_wday = bin_align(read_cmos_register(CMOS_REG_WDAY));
    _tm.tm_mon  = bin_align(read_cmos_register(CMOS_REG_MON));
    _tm.tm_year = bin_align(read_cmos_register(CMOS_REG_YEAR));
    read_cmos_register(CMOS_REG_C);
}
void install_interrupt(){
    __asm__ __volatile__("cli");
    uint8_t status_b = read_cmos_register(CMOS_REG_B);
    write_cmos_register(CMOS_REG_B,status_b | 0x40); //SET BIT 6 (zero based)
    __asm__ __volatile__("sti");

}
void rtc_install(){
    register_interrupt_handler(IRQ_BASE + IRQ8_CMOS_CLOCK,handle_rtc_inturrept);
    uint8_t status_b = read_cmos_register(CMOS_REG_B);
    is_24h = BITREAD(status_b,1);
    is_bin = BITREAD(status_b,2);
    install_interrupt();
}