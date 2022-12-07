#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>

const int HEADER_SIZE = 12;
const int BLOCK_SIZE = 4;
const int NEXT_PTR = 4;
const int PREV_PTR = 4;
const int FOOTER_SIZE = 4;
const int MEMORY_SIZE = 64;
const int ALIGNMENT = 8;
const int NULL_PTR = 0;

static unsigned char *heap = NULL;
static int alg = -1;
static int root = NULL_PTR;

static unsigned char searchPtr = NULL_PTR;

// smallest free block will be 16 btyes

void printHeap()
{
  printf("Heap: ");
  for (int i = 0; i < MEMORY_SIZE; i++)
  {
    printf("%d ", heap[i]);
  }
  printf("\n");
}

// returns the pointer after the 4 blocks of int
int insertInt(int num, int insertPos, unsigned char *heap)
{
  for (int i = 24; i >= 0; i -= 8)
  {
    unsigned char c = num >> i;
    heap[insertPos] = c;
    insertPos++;
  }
  return insertPos + 1;
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

int getBlockSize(int p, unsigned char *heap)
{
  return readInt(p - HEADER_SIZE, heap);
}

int getNextPtr(int p, unsigned char *heap)
{
  return readInt(p - HEADER_SIZE + BLOCK_SIZE, heap);
}

int getPrevPtr(int p, unsigned char *heap)
{
  return readInt(p - HEADER_SIZE + BLOCK_SIZE + NEXT_PTR, heap);
}

void setHeaderSize(int p, int newSize, unsigned char *heap)
{
  insertInt(newSize, p - HEADER_SIZE, heap);
}

void setNextPtr(int p, int newNext, unsigned char *heap)
{
  insertInt(newNext, p - HEADER_SIZE + BLOCK_SIZE, heap);
}

void setPrevPtr(int p, int newPrev, unsigned char *heap)
{
  insertInt(newPrev, p - HEADER_SIZE + BLOCK_SIZE + NEXT_PTR, heap);
}

void setFooterSize(int p, int blockSize, unsigned char *heap)
{
  insertInt(blockSize, p + blockSize - HEADER_SIZE - FOOTER_SIZE, heap);
}

size_t spaceCalc(size_t size)
{
  size_t res = BLOCK_SIZE + FOOTER_SIZE + size;
  int padding = res % ALIGNMENT != 0 ? ALIGNMENT - res % ALIGNMENT : 0;
  return res + padding;
}

// returns pos right after header of split block
int splitBlock(int p, int spaceNeeded, int chosenBlockSize, unsigned char *heap)
{
  printf("in split block\n");
  int splitSize = chosenBlockSize - spaceNeeded;
  int split_p;
  setHeaderSize(p, spaceNeeded, heap);
  setFooterSize(p, spaceNeeded, heap);
  if (splitSize < 16)
  {
    split_p = p - NEXT_PTR - PREV_PTR + spaceNeeded;
    insertInt(ALIGNMENT, split_p - BLOCK_SIZE, heap);
    insertInt(ALIGNMENT, split_p, heap);
    return p;
  }

  split_p = p + spaceNeeded;
  setHeaderSize(split_p, splitSize, heap);
  setFooterSize(split_p, splitSize, heap);
  setNextPtr(split_p, getNextPtr(p, heap), heap);
  setPrevPtr(split_p, getPrevPtr(p, heap), heap);
  return split_p;
}

void updatePtrs(int p, bool blockWasSplit, unsigned char *heap)
{
  printf("in update ptrs\n");
  // int size = getBlockSize(p, heap);
  int next = getNextPtr(p, heap);
  int prev = getPrevPtr(p, heap);
  printf("next: %d\n", next);
  printf("prev: %d\n", prev);
  if (blockWasSplit)
  {
    if (prev == NULL_PTR)
      root = p;
    else if (next == NULL_PTR)
      setNextPtr(prev, p, heap);
    else
    {
      setNextPtr(prev, p, heap);
      setPrevPtr(next, p, heap);
    }
  }
  else
  {
    if (next == NULL_PTR && prev == NULL_PTR)
      root = NULL_PTR;
    else if (next == NULL_PTR)
      setNextPtr(prev, NULL_PTR, heap);
    else if (prev == NULL_PTR)
    {
      root = next;
      setPrevPtr(next, NULL_PTR, heap);
    }
    else
    {
      setNextPtr(prev, next, heap);
      setPrevPtr(next, prev, heap);
    }
  }
}

void myinit(int allocAlg)
{
  int p = HEADER_SIZE;
  heap = calloc(sizeof(unsigned char), MEMORY_SIZE);
  setHeaderSize(p, MEMORY_SIZE, heap);
  setNextPtr(p, NULL_PTR, heap);
  setPrevPtr(p, NULL_PTR, heap);
  setFooterSize(p, MEMORY_SIZE, heap);
  root = p;
  alg = allocAlg;
}

void *mymalloc(size_t size)
{
  if (size == 0)
    return NULL;
  size_t spaceNeeded = spaceCalc(size);
  printf("spaceNeeded: %lu\n", spaceNeeded);
  if (alg == 0)
  {
    int p = root;
    while (p != NULL_PTR && getBlockSize(p, heap) <= spaceNeeded)
      p = getNextPtr(p, heap);

    if (p == NULL_PTR)
      return NULL;

    int chosenBlockSize = getBlockSize(p, heap);
    int update_p = p;
    if (spaceNeeded < chosenBlockSize)
      update_p = splitBlock(p, spaceNeeded, chosenBlockSize, heap);
    bool blockWasSplit = update_p != p ? true : false;
    printf("blockWasSplit: %d\n", blockWasSplit);
    printf("update_p: %d\n", update_p);
    updatePtrs(update_p, blockWasSplit, heap);
    printHeap();
    printf("root: %d\n", root);
    return heap + p;
  }
  return NULL;
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

// what happens if the split part has less space than 16 bytes (the amount needed to store metadata)?
