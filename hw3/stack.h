#pragma once

#include "process.h"

typedef struct ListNode ListNode;

struct ListNode
{
  Process *element;
  ListNode *next;
};

typedef struct Stack
{
  ListNode *head;
} Stack;

Process *pop(Stack *stack);
Process *getElem(Stack *stack, int jid);
Process *removeElem(Stack *stack, int jid);
void push(Stack *stack, Process *process);
void printStack(Stack *stack);
void freeStack(Stack *stack);
