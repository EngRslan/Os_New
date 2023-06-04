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
void ScheduleHandle(){
    if(nextSchedule){
        //TODO TO MAKE SEARCH MORE FASTER
        Schedule *currentServe = nextSchedule;
                __asm__("cli");
        while (currentServe)
        {
            if(currentServe->nextExecution < ticks){
                currentServe->callback();
                currentServe->nextExecution = ticks + currentServe->interval;
            }
            currentServe = currentServe->next;
        }
                __asm__("sti");
    }
    
}

void ScheduleInterval(ScheduleCallback callback,uint32_t interval){
    __asm__("cli");
        Schedule *schedule = (Schedule *)kmalloc(sizeof(Schedule));
        schedule->callback = callback;
        schedule->interval = interval;
        schedule->nextExecution = Millis() + interval;

    if(nextSchedule == NULL){
        nextSchedule = schedule;
        __asm__("sti");
        return;
    }

    Schedule *prev;
    Schedule *current = nextSchedule;
    while (1)
    {
        if(current->nextExecution > schedule->nextExecution){
            schedule->next = current;
            if(prev != NULL)
            {
                prev->next = schedule;
            }
            else
            {
                nextSchedule = schedule;
            }
            break;
        }
        

        prev = current;
        current = current->next;

        if(current == NULL){
            prev->next = schedule;
            break;
        }
    }
    __asm__("sti");
    
}
