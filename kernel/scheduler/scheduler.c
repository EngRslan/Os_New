#include <kernel/scheduler/scheduler.h>
#include <kernel/mem/kheap.h>
#include <kernel/types.h>
#include <stddef.h>

typedef struct Schedule{
    ScheduleCallback callback;
    uint32_t interval;
    uint64_t nextExecution;
    struct Schedule *next;
} Schedule;

Schedule *nextSchedule = NULL;
extern uint64_t ticks;
uint32_t Millis(){
    return ticks;
}
void ScheduleInterval(ScheduleCallback callback,uint32_t interval){
        Schedule *schedule = (Schedule *)kmalloc(sizeof(Schedule));
        schedule->callback = callback;
        schedule->interval = interval;
        schedule->nextExecution = Millis() + interval;

    if(nextSchedule == NULL){
        nextSchedule = schedule;
        return;
    }

    Schedule *prev;
    Schedule *next;
    while (1)
    {
        if(next->nextExecution > schedule->nextExecution){
            schedule->next=next;
            prev->next = schedule;
            break;
        }
        

        prev = next;
        next = next->next;
    }
    (nextSchedule->nextExecution < schedule->nextExecution){
        
    }
    
}
