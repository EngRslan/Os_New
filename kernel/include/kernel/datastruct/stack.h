#ifndef STACK_H
#define STACK_H
#include <kernel/types.h>
#include <stdbool.h>

typedef struct Stack
{
    int pointer;
    uint32_t size;
    int *buffer;
}Stack;

Stack *StackCreate(uint32_t initial);
void StackDestroy(Stack *);
void StackPush(Stack *stack, int value);
int StackPop(Stack *stack);
bool StackIsEmpty(Stack *stack);
bool StackIsFull(Stack *stack);

#endif