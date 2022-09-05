#ifndef RTC_H
#define RTC_H
#include <kernel/types.h>

#define PORT_REGISTER   0x70
#define PORT_DATA       0x71

#define CMOS_REG_SECONDS    0x00
#define CMOS_REG_MINUTES    0x02
#define CMOS_REG_HOURS      0X04
#define CMOS_REG_WDAY       0X06
#define CMOS_REG_MDAY       0X07
#define CMOS_REG_MON        0X08
#define CMOS_REG_YEAR       0X09
#define CMOS_REG_CENTURY    0X32
#define CMOS_REG_A          0X0A
#define CMOS_REG_B          0X0B
#define CMOS_REG_C          0X0C

typedef struct tm
{
    int32_t tm_sec;     // seconds, 0 to 59
    int32_t tm_min;     // minutes, 0 to 59
    int32_t tm_hour;    // hours, 0 to 23
    int32_t tm_mday;    // day of the month, 0 to 31
    int32_t tm_mon;     // month 0 to 11
    int32_t tm_year;    // year years since 1900;
    int32_t tm_wday;    // day of week, 0 to 6
    int32_t tm_yday;    // day of year, 0 to 365
    int32_t tm_isdst;   // daylight saving time
};

void rtc_install();
#endif