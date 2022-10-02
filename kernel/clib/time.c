#include <ktime.h>
#include <kernel/drivers/rtc.h>
#include <stddef.h>

int g_local_timezone = +3 * 60 ;//Riyadh Time zone


void TimeNow(Time *time){
*time = _tm.tm_sec +
        _tm.tm_min * 60 +
        _tm.tm_hour * 3600 +
        _tm.tm_yday * 86400 +
        (_tm.tm_year - 70) * 31536000 +
        ((_tm.tm_year - 69) / 4) * 86400 -
        ((_tm.tm_year - 1) / 100) * 86400 +
        ((_tm.tm_year + 299) / 400) * 86400;
}
DateTime *LocalTime(Time *time){
return NULL;
}