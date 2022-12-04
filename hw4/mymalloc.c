#include <stdlib.h>
#include <math.h>

const HEADER_SIZE = 12;
const FOOTER_SIZE = 4;
const MEMORY_SIZE = 1000000;
const NULL_PTR = -1;

static char *heap = NULL;
static int alg = -1;
static int root = NULL_PTR;

// for next fit
static char searchPtr = NULL_PTR;

// returns the pointer after the 4 blocks of int
int insertInt(int num, int insertPos, char *heap)
{
  int mask = (1 << 8) - 1;
  for (int i = 24, j = 0; i >= 0; i -= 8, j++)
  {
    char c = ((num >> i) & mask) + '0';
    heap[insertPos] = c;
    insertPos + j;
  }
  return insertPos + 1;
}

int getInt(int ptr, char *heap)
{
  int res = 0;
  for (int i = 3; i >= 0; i--)
  {
    res += heap[ptr] * (int)pow(8, i);
    ptr++;
  }
  return res;
}

void myinit(int allocAlg)
{
  heap = calloc(sizeof(char), MEMORY_SIZE);
  int insertPos = 0;
  // insert size in front
  insertPos = insertInt(MEMORY_SIZE, 0, heap);
  // insert next ptr
  insertPos = insertInt(NULL_PTR, insertPos, heap);
  // insert prev ptr
  insertPos = insertInt(NULL_PTR, insertPos, heap);
  // insert size in end
  insertInt(MEMORY_SIZE, MEMORY_SIZE - FOOTER_SIZE, heap);
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
  root = NULL_PTR;
  searchPtr = NULL_PTR;
  alg = -1;
}
