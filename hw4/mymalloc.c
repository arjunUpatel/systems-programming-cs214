#include <stdlib.h>
#include <math.h>

const HEADER_SIZE = 12;
const BLOCK_SIZE = 4;
const NEXT_PTR = 4;
const PREV_PTR = 4;
const FOOTER_SIZE = 4;
const MEMORY_SIZE = 1000000;
const ALIGNMENT = 8;
const NULL_PTR = -1;

static char *heap = NULL;
static int alg = -1;
static int root = NULL_PTR;

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

int getBlockSize(int p, char *heap)
{
  return getInt(p - HEADER_SIZE, heap);
}

int getNextPtr(int p, char *heap)
{
  return getInt(p - HEADER_SIZE - BLOCK_SIZE, heap);
}

int getPrevPtr(int p, char *heap)
{
  return getInt(p - HEADER_SIZE - BLOCK_SIZE - NEXT_PTR, heap);
}

size_t spaceCalc(size_t size)
{
  size_t res = HEADER_SIZE + FOOTER_SIZE + size;
  int padding = res % ALIGNMENT != 0 ? ALIGNMENT - res % ALIGNMENT : 0;
  return res + padding;
}

// return pos right after header of split block
int splitBlock(int p, int spaceNeeded, int chosenBlockSize, char *heap)
{
  int splitSize = chosenBlockSize - spaceNeeded;
  // change size of chosen block in header
  insertInt(spaceNeeded, p - HEADER_SIZE, heap);
  // update size of chosen block in footer
  int insertPos = insertInt(spaceNeeded, p + spaceNeeded - FOOTER_SIZE, heap);
  // change size of split block in footer
  insertInt(splitSize, insertPos + splitSize - FOOTER_SIZE, heap);
  // change size of split block in header
  insertPos = insertInt(splitSize, insertPos, heap);
  // copy next ptr into split
  insertPos = insertInt(getInt(p + BLOCK_SIZE, heap), insertPos, heap);
  // copy prev ptr into split
  return insertInt(getInt(p + BLOCK_SIZE + NEXT_PTR, heap), insertPos, heap);
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

void *mymalloc(size_t size)
{
  if (size == 0)
    return NULL;
  size_t spaceNeeded = spaceCalc(size);
  if (alg == 0)
  {
    int p = root;
    while (p != NULL_PTR && getBlockSize(p, heap) <= spaceNeeded)
      p = getNextPtr(p, heap);

    // spaceNeeded less than chosen block size
    int chosenBlockSize = getBlockSize(p, heap);
    int update_p = p;
    if (spaceNeeded < chosenBlockSize)
      update_p = splitBlock(p, spaceNeeded, chosenBlockSize, heap);
    // TODO: update links of prev and next block
    // if (update_p == p)
    // {
    //   // make next and prev skip curr block
    //   // cases
    //   // next null
    //   // prev null
    //   //
    // }
    // else
    // {
    //   // make next and prev point to curr block
    //   // update next of prev
    //   if (getNextPtr(update_p, heap) != -1)
    //     insertInt(update_p, getNextPtr(update_p, heap), heap);
    //   if (getPrevPtr(update_p, heap) != -1)
    //     insertInt(update_p, getPrevPtr(update_p, heap), heap);
    // }
    return heap + p;
  }
}

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
