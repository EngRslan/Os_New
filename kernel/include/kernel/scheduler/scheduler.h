#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <kernel/types.h>
typedef void (*ScheduleCallback)(void);

uint32_t Millis();
void ScheduleInterval(ScheduleCallback callback,uint32_t interval);
void ScheduleHandle();
#endif