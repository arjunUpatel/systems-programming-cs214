#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

const int FREE_HEADER_SIZE = 12;
const int SIZE_HEADER = 4;
const int NEXT_PTR = 4;
const int PREV_PTR = 4;
const int FOOTER_SIZE = 4;
const int MEMORY_SIZE = 64;
const int ALIGNMENT = 8;
const int NULL_PTR = -1;

static unsigned char *heap = NULL;
static int alg = -1;
static int root = NULL_PTR;
static unsigned char searchPtr = NULL_PTR;

void printHeap()
{
  printf("Heap: ");
  for (int i = 0; i < MEMORY_SIZE; i++)
  {
    printf("%d ", heap[i]);
  }
  printf("\n");
}

int insertInt(int value, int pos, unsigned char *heap)
{
  for (int i = 24; i >= 0; i -= 8)
  {
    unsigned char c = value >> i;
    heap[pos] = c;
    pos++;
  }
  return pos;
}

int readInt(int pos, unsigned char *heap)
{
  int result = 0;
  for (int i = 0; i < 4; i++)
  {
    for (int bit = sizeof(char) * 8 - 1; 0 <= bit; bit--)
    {
      size_t bit_val = ((1 << 1) - 1) & heap[pos] >> bit * 1; // shift and mask
      result += bit_val ? 1 : 0;
      result = i != 3 || bit != 0 ? result << 1 : result;
    }
    pos++;
  }
  return result;
}

int getNextPtr(int pos, unsigned char *heap)
{
  return readInt(pos + SIZE_HEADER, heap);
}

int getPrevPtr(int pos, unsigned char *heap)
{
  return readInt(pos + SIZE_HEADER + NEXT_PTR, heap);
}

int getBlockSize(int pos, unsigned char *heap)
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

  return result;
}

bool getIsAllocated(int pos, unsigned char *heap)
{
  return heap[pos + 3] & 1;
}

void setNextPtr(int pos, int next, unsigned char *heap)
{
  insertInt(next, pos + SIZE_HEADER, heap);
}

void setPrevPtr(int pos, int prev, unsigned char *heap)
{
  insertInt(prev, pos + SIZE_HEADER + NEXT_PTR, heap);
}

int setSizeHeader(int pos, int size, bool isAllocated, unsigned char *heap)
{
  int value = size;
  if (isAllocated)
    value += 1;
  return insertInt(value, pos, heap);
}

int setFooter(int pos, int size, bool isAllocated, unsigned char *heap)
{
  int value = size;
  if (isAllocated)
    value += 1;
  return insertInt(value, pos + size - FOOTER_SIZE, heap);
}

// TODO: Use to add padding to allocated blocks
int calculateSpace(int size)
{
  int res = SIZE_HEADER + FOOTER_SIZE + size;
  int padding = res % ALIGNMENT != 0 ? ALIGNMENT - res % ALIGNMENT : 0;
  return res + padding;
}

int splitBlock(int pos, int spaceNeeded, int chosenBlockSize, unsigned char *heap)
{
  printf("in split block\n");
  setSizeHeader(pos, spaceNeeded, true, heap);
  setFooter(pos, spaceNeeded, true, heap);
  if (spaceNeeded < chosenBlockSize)
  {
    printf("gothere\n");
    int splitSize = chosenBlockSize - spaceNeeded;
    int split_p = pos + spaceNeeded;
    if (splitSize < 16)
    {
      setSizeHeader(split_p, ALIGNMENT, false, heap);
      setFooter(split_p, ALIGNMENT, false, heap);
      return pos;
    }

    setSizeHeader(split_p, splitSize, false, heap);
    setFooter(split_p, splitSize, false, heap);
    setNextPtr(split_p, getNextPtr(pos, heap), heap);
    setPrevPtr(split_p, getPrevPtr(pos, heap), heap);
    return split_p;
  }
  return pos;
}

void updatePtrs(int pos, bool blockWasSplit, unsigned char *heap)
{
  printf("in update ptrs\n");
  int next = getNextPtr(pos, heap);
  int prev = getPrevPtr(pos, heap);
  printf("next: %d\n", next);
  printf("prev: %d\n", prev);
  if (blockWasSplit)
  {
    if (prev == NULL_PTR)
      root = pos;
    else if (next == NULL_PTR)
      setNextPtr(prev, pos, heap);
    else
    {
      setNextPtr(prev, pos, heap);
      setPrevPtr(next, pos, heap);
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
  heap = calloc(sizeof(unsigned char), MEMORY_SIZE);
  setSizeHeader(0, MEMORY_SIZE, false, heap);
  setNextPtr(0, NULL_PTR, heap);
  setPrevPtr(0, NULL_PTR, heap);
  setFooter(0, MEMORY_SIZE, false, heap);
  root = 0;
  alg = allocAlg;
}

void *mymalloc(size_t size)
{
  if (size == 0 || root == NULL_PTR)
    return NULL;
  int p;
  size_t spaceNeeded = calculateSpace(size);
  printf("spaceNeeded: %lu\n", spaceNeeded);
  if (alg == 0)
  {
    p = root;
    while (p != NULL_PTR && getBlockSize(p, heap) < spaceNeeded)
      p = getNextPtr(p, heap);
    if (p == NULL_PTR)
      return NULL;
  }
  else if (alg == 1)
  {
    p = searchPtr;
    if (getBlockSize(p, heap) < spaceNeeded)
    {
      int stop_p = p;
      do
      {
        p = getNextPtr(p, heap);
        if (p == NULL_PTR)
          p = root;
      } while (p != stop_p && getBlockSize(p, heap) < spaceNeeded);

      if (p == stop_p)
        return NULL;
    }
  }
  else if (alg == 2)
  {
    // best fit stuff
  }
  else
    return NULL;
  int chosenBlockSize = getBlockSize(p, heap);
  int update_p = splitBlock(p, spaceNeeded, chosenBlockSize, heap);
  bool blockWasSplit = update_p != p ? true : false;
  printf("blockWasSplit: %d\n", blockWasSplit);
  printf("update_p: %d\n", update_p);
  printHeap();
  updatePtrs(update_p, blockWasSplit, heap);
  printf("root: %d\n", root);
  if (alg == 2)
  {
    if (blockWasSplit)
      searchPtr = update_p;
    else
      searchPtr = getNextPtr(update_p, heap);
    if (searchPtr == NULL_PTR)
      searchPtr = root;
  }
  return heap + p + SIZE_HEADER;
}

// Converting void pointers to char
// void *p = &heap[pos];
void myfree(void *ptr)
{
  if (ptr == NULL)
    return;

  int pos = (unsigned char *)ptr - heap;
  int size = getBlockSize(pos, heap);

  if (size >= FREE_HEADER_SIZE + FOOTER_SIZE)
  {
    // setSizeHeader(pos, size, heap);
    // setNextPtr(pos, next, heap);
    // setPrevPtr(pos, prev, heap);
    // setFooter(pos, size, heap);
  }
}

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
