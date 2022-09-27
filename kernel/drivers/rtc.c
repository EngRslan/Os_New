#include <kernel/drivers/rtc.h>
#include <kernel/system.h>
#include <kernel/isr.h>
#include <kernel/types.h>
#include <kernel/bits.h>
#include <stdio.h>

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
    if(is_24h == 0){
        uint8_t is_pm = BITREAD(num,7);
        uint8_t hour = num & 0x7F;
        if(num == 12)return 0;
        if(!is_pm) return num;
        return num + 12;
    }
    return num;
}
void handle_rtc_inturrept(register_t * reg){
    _tm.tm_sec  = bin_align(read_cmos_register(CMOS_REG_SECONDS));
    _tm.tm_min  = bin_align(read_cmos_register(CMOS_REG_MINUTES));
    _tm.tm_hour = format_align(bin_align(read_cmos_register(CMOS_REG_HOURS)));
    _tm.tm_mday = bin_align(read_cmos_register(CMOS_REG_MDAY));
    _tm.tm_wday = bin_align(read_cmos_register(CMOS_REG_WDAY));
    _tm.tm_mon  = bin_align(read_cmos_register(CMOS_REG_MON));
    uint8_t year = bin_align(read_cmos_register(CMOS_REG_YEAR));
    uint8_t century = bin_align(read_cmos_register(CMOS_REG_CENTURY));
    _tm.tm_year = ((century * 100) + century - 1900);
    _tm.tm_isdst = 0;
    read_cmos_register(CMOS_REG_C);
}

void str_date(char * str){
    sprintf(str,"%d/%02d/%02d %02d:%02d:%02d",_tm.tm_year+1900,_tm.tm_mon,_tm.tm_mday,_tm.tm_hour,_tm.tm_min,_tm.tm_sec);
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