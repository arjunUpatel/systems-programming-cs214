#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
  // 160+134*2^8+1*2^16
  int result = 0;
  for (int i = 24; i >= 0; i -= 8)
  {
    result += heap[pos] * pow(2.0, i);
    pos++;
  }
  return result;
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
  // inster prev ptr
  insertPos = insertInt(1, insertPos, heap);
  // insert size in end
  insertInt(heapSize, heapSize - 1 - 3, heap);
  printHeap(heapSize);
  root = insertPos;
  alg = allocAlg;

  printf("Read pos 0: %d\n", readInt(0, heap));
  printf("Read pos 4: %d\n", readInt(4, heap));
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