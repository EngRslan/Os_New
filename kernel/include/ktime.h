#ifndef TIME_H
#define TIME_H
#include <kernel/types.h>
typedef uint32_t Time;
extern int g_local_timezone;

typedef struct DateTime
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t mday;
    uint8_t month;
    uint8_t wkday;
    uint16_t year;
    uint16_t yday;
    int tzOffset;
} DateTime;

void TimeNow(Time *time);
DateTime *LocalTime(Time *time);

#endif