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
  ListNode *currNode = stack->head;
  while (currNode != NULL)
  {
    if (currNode->element->jid == jid)
    {
      if (prevNode != NULL)
        prevNode->next = currNode->next;
      else
        stack->head = currNode->next;
      Process *res = currNode->element;
      free(currNode);
      return res;
    }
    prevNode = currNode;
    currNode = currNode->next;
  }
  return NULL;
}

Process *getElem(Stack *stack, int jid)
{
  ListNode *currNode = stack->head;
  while (currNode != NULL)
  {
    if (currNode->element->jid == jid)
      return currNode->element;
    currNode = currNode->next;
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

void printStack(Stack *stack)
{
  ListNode *stackHead = stack->head;

  Stack tempStack;
  tempStack.head = NULL;
  while (stack->head != NULL)
  {
    push(&tempStack, stack->head->element);
    stack->head = stack->head->next;
  }

  while (tempStack.head != NULL)
    printJob(pop(&tempStack));

  stack->head = stackHead;
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
