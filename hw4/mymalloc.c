#include <stdlib.h>
#include <math.h>

const HEADER_SIZE = 12;
const BLOCK_SIZE = 4;
const NEXT_PTR = 4;
const PREV_PTR = 4;
const FOOTER_SIZE = 4;
const MEMORY_SIZE = 1024 * 1024;
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
  return getInt(p - HEADER_SIZE + BLOCK_SIZE, heap);
}

int getPrevPtr(int p, char *heap)
{
  return getInt(p - HEADER_SIZE + BLOCK_SIZE + NEXT_PTR, heap);
}

void setHeaderSize(int p, int newSize, char *heap)
{
  insertInt(newSize, p - HEADER_SIZE, heap);
}

void setNextPtr(int p, int newNext, char *heap)
{
  insertInt(newNext, p - HEADER_SIZE + BLOCK_SIZE, heap);
}

void setPrevPtr(int p, int newPrev, char *heap)
{
  insertInt(newPrev, p - HEADER_SIZE + BLOCK_SIZE + NEXT_PTR, heap);
}

void setFooterSize(int p, int blockSize, char *heap)
{
  insertInt(blockSize, p + blockSize - HEADER_SIZE - FOOTER_SIZE, heap);
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
  int split_p = p + spaceNeeded + HEADER_SIZE;
  // change size of chosen block in header
  setHeaderSize(p, spaceNeeded, heap);
  // update size of chosen block in footer
  setFooterSize(p, spaceNeeded, heap);
  // change size of split block in header
  setHeaderSize(split_p, splitSize, heap);
  // change size of split block in footer
  setFooterSize(split_p, splitSize, heap);
  // copy next ptr into split
  setNextPtr(split_p, getNextPtr(p, heap), heap);
  // copy prev ptr into split
  setPrevPtr(split_p, getPrevPtr(p, heap), heap);
  return split_p;
}

void myinit(int allocAlg)
{
  int p = HEADER_SIZE;
  heap = calloc(sizeof(char), MEMORY_SIZE);
  // insert size in front
  setHeaderSize(p, MEMORY_SIZE, heap);
  // insert next ptr
  setNextPtr(p, NULL_PTR, heap);
  // insert prev ptr
  setPrevPtr(p, NULL_PTR, heap);
  // insert size in end
  setFooterSize(p, MEMORY_SIZE, heap);
  root = p;
  alg = allocAlg;
}

void updatePtrs(int p, int blockWasSplit, char *heap)
{
  if (blockWasSplit)
  {
    if (getNextPtr(p, heap) == -1)
      setNextPtr(getPrevPtr(p, heap), p, heap);
    if (getPrevPtr(p, heap) == -1)
      root = getNextPtr(p, heap);
    else
    {
      setNextPtr(getPrevPtr(p, heap), getNextPtr(p, heap), heap);
      setPrevPtr(getNextPtr(p, heap), getPrevPtr(p, heap), heap);
    }
  }
  else
  {
    if (getNextPtr(p, heap) == -1)
      setNextPtr(getPrevPtr(p, heap), NULL_PTR, heap);
    if (getPrevPtr(p, heap) == -1)
      root = getNextPtr(p, heap);
    else
    {
      setNextPtr(getPrevPtr(p, heap), p, heap);
      setPrevPtr(getNextPtr(p, heap), p, heap);
    }
  }
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
    int blockWasSplit = update_p == p ? 1 : 0;
    updatePtrs(update_p, blockWasSplit, heap);
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
