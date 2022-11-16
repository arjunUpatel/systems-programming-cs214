#include <stdlib.h>
#include "stack.h"
#include "process.h"

Process *pop(Stack *stack)
{
    if (stack->head == NULL)
        return NULL;
    Process *res = stack->head->element;
    ListNode *tempNode = stack->head;
    stack->head = stack->head->next;
    free(tempNode);
    return res;
}

Process *removeElem(Stack *stack, int jid)
{
    ListNode *prevNode = NULL;
    while (stack->head != NULL)
    {
        if (stack->head->element->jid == jid)
        {
            prevNode->next = stack->head->next;
            Process *res = stack->head->element;
            free(stack->head);
            return res;
        }
        prevNode = stack->head;
        stack->head = stack->head->next;
    }
    return NULL;
}

void push(Stack *stack, Process *process)
{
    ListNode *newNode = malloc(sizeof(ListNode));
    newNode->element = process;
    newNode->next = stack->head;
    stack->head = newNode;
}

// void put(Stack *stack, Process *process, int jid)
// {

//    while() 
// }

void printStack(Stack *stack)
{
    Stack tempStack;
    tempStack.head = NULL;
    while (stack->head != NULL)
    {
        push(&tempStack, stack->head->element);
        stack->head = stack->head->next;
    }

    while (tempStack.head != NULL)
        printJob(pop(&tempStack));
}

void freeStack(Stack *stack)
{
    while (stack->head != NULL)
    {
        Process *p = pop(stack);
        freeProcess(p);
    }
    free(stack);
}
