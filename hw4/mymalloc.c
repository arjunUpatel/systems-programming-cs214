#include <stdlib.h>

typedef struct Header
{
  int size;
  struct Header *next;
} Header;

typedef struct Footer
{
  int size;
} Footer;

const HEADER_SIZE = 12;
const FOOTER_SIZE = 4;
const MEMORY_SIZE = 1000000;

void *heap;
int alg;

void myinit(int allocAlg)
{
  // Create header for heap
  heap = calloc(sizeof(char), MEMORY_SIZE + HEADER_SIZE);
  Header *heapHeader = heap;
  heapHeader->size = MEMORY_SIZE;

  Header *blockHeader = heapHeader + 1;
  blockHeader->size = heapHeader->size - HEADER_SIZE - FOOTER_SIZE;
  blockHeader->next = NULL;

  // TODO: Create footer for block

  heapHeader->next = blockHeader;

  alg = allocAlg;
}

void *mymalloc(size_t size)
{
  if (size == 0)
    return NULL;

  Header *curr = ((Header *)heap)->next;
  Header *prev = (Header *)heap;

  // Find free block that is large enough
  while (curr != NULL && curr->size < size)
  {
    prev = curr;
    curr = curr->next;
  }

  // Could not find free block
  if (curr == NULL)
    return NULL;

  // Remove free block from list
  prev->next = curr->next;
  curr->next = NULL;

  // TODO: Split free block to used block and free block

  return curr;
}

void myfree(void *ptr)
{
}
void *myrealloc(void *ptr, size_t size)
{
}

void mycleanup()
{
}
