#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

static unsigned char *heap = NULL;
static int alg = -1;
static int root = -1;
static char *searchPtr = NULL;

// returns the pointer after the 4 blocks of int
int insertInt(int num, int insertPos, unsigned char *heap)
{
  for (int i = 24; i >= 0; i -= 8)
  {
    unsigned char c = num >> i;
    heap[insertPos] = c;
    insertPos++;
  }
  return insertPos;
}

void printHeap(int heapSize)
{
  for (int i = 0; i < heapSize; i++)
  {
    printf("%d ", heap[i]);
  }
  printf("\n");
}

void myinit(int allocAlg)
{
  int heapSize = 32;
  heap = calloc(sizeof(unsigned char), heapSize);
  int insertPos = 0;
  // insert size in front
  insertPos = insertInt(heapSize, 0, heap);
  printHeap(heapSize);
  // insert next ptr
  insertPos = insertInt(100000, insertPos, heap);
  printHeap(heapSize);
  // inster prev ptr
  insertPos = insertInt(1, insertPos, heap);
  printHeap(heapSize);
  // insert size in end
  insertInt(heapSize, heapSize - 1 - 3, heap);
  printHeap(heapSize);
  root = insertPos;
  alg = allocAlg;
}

void *mymalloc(size_t size);
void myfree(void *ptr);
void *myrealloc(void *ptr, size_t size);

void mycleanup()
{
  free(heap);
  heap = NULL;
  root = -1;
  searchPtr = NULL;
  alg = -1;
}