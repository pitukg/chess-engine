#include "stack.h"


void push(StackObject stObj, Stack *stack) {
    stack -> stackArray[stack->size] = stObj;
    stack -> size++;
}

StackObject pop(Stack *stack) {
    stack -> size--;
    return stack->stackArray[stack->size];
}

