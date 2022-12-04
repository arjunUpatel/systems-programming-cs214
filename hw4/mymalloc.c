#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

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

int readInt(int pos, unsigned char *heap)
{
  int result = 0;
  for (int i = 24; i >= 0; i -= 8)
  {
    result += heap[pos] * pow(2.0, i);
    pos++;
  }
  return result;
}

int insertSize(int size, bool isAllocated, int insertPos, unsigned char *heap)
{
  int value = size << 1;
  if (isAllocated)
    value += 1;
  return insertInt(value, insertPos, heap);
}

int readSize(int pos, unsigned char *heap)
{
  int mask = 255 << 1;
  int result = 0;
  for (int i = 24; i >= 0; i -= 8)
  {
    // Mask out last bit and calculate size
    if (i == 0)
      result += (heap[pos] & mask);
    else
      result += heap[pos] * pow(2.0, i);
    pos++;
  }

  // Shift result due to masking out last bit
  return result >> 1;
}

bool readIsAllocated(int pos, unsigned char *heap)
{
  // Return value of isAllocated bit
  return heap[pos + 3] & 1;
}

void printHeap(int heapSize)
{
  printf("Heap: ");
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
  // insert next ptr
  insertPos = insertInt(100000, insertPos, heap);
  // insert prev ptr
  insertPos = insertInt(1, insertPos, heap);
  // insert size header
  insertPos = insertSize(1000, true, insertPos, heap);
  // insert size in end
  insertInt(heapSize, heapSize - 1 - 3, heap);
  printHeap(heapSize);
  root = insertPos;
  alg = allocAlg;

  printf("Read pos 0: %d\n", readInt(0, heap));
  printf("Read pos 4: %d\n", readInt(4, heap));
  printf("Read pos 8: %d\n", readInt(8, heap));
  printf("Read pos 12: %d\n", readSize(12, heap));
  printf("Read pos 12 allocated: %d\n", readIsAllocated(12, heap));
  printf("Read pos 16: %d\n", readInt(16, heap));
  printf("Read pos 20: %d\n", readInt(20, heap));
  printf("Read pos 24: %d\n", readInt(24, heap));
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