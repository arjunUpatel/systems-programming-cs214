#include <stdlib.h>

const HEADER_SIZE = 12;
const FOOTER_SIZE = 4;
const MEMORY_SIZE = 1000000;

static void *heap = NULL;
static int alg = -1;
static int root = -1;

// for next fit
static char *searchPtr = NULL;

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

void myinit(int allocAlg)
{
  heap = calloc(sizeof(char), HEADER_SIZE);
  int insertPos = 0;

  // insert size in front
  insertPos = insertInt(1000000, 0, heap);
  // insert next ptr

  insertPos = insertInt(-1, insertPos, heap);

  // inster prev ptr
  insertPos = insertInt(-1, insertPos, heap);
  
  // insert size in end
  insertInt(1000000, 1000000 - 1 - 3, heap);
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
