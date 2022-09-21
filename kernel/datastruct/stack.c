#include <kernel/datastruct/stack.h>
#include <kernel/mem/kheap.h>
#include <kernel/types.h>
#include <logger.h>
#include <stdbool.h>
#include <string.h>

Stack *StackCreate(uint32_t size){
    Stack *stack = (Stack *)kmalloc(sizeof(Stack));
    stack->buffer = (int *)kmalloc(size * sizeof(int));
    memset((void *)stack->buffer,0,size * sizeof(int));
    stack->size = size;
    stack->pointer = stack->size - 1;
    return stack;
}
void StackDestroy(Stack *stack){
    kfree(stack->buffer);
    kfree(stack);
}
void StackPush(Stack *stack, int value){
    if(StackIsFull(stack)){
        log_warning("Stack Full");
        return;
    }
    stack->buffer[stack->pointer--] = value;
}
int StackPop(Stack *stack){
    if(StackIsEmpty(stack)){
        log_warning("Stack Empty");
        return;
    }
    int v = stack->buffer[++stack->pointer];
    stack->buffer[stack->pointer] = 0;
    return v;
}
bool StackIsEmpty(Stack *stack){
    return stack->pointer == stack->size;
}
bool StackIsFull(Stack *stack){
    return stack->pointer == 0;
}