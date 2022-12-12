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

int calculateSpace(int size)
{
  int res = SIZE_HEADER + FOOTER_SIZE + size;
  int padding = res % ALIGNMENT != 0 ? ALIGNMENT - res % ALIGNMENT : 0;
  return res + padding;
}

int splitBlock(int pos, int spaceNeeded, int chosenBlockSize, unsigned char *heap)
{
  setSizeHeader(pos, spaceNeeded, true, heap);
  setFooter(pos, spaceNeeded, true, heap);
  if (spaceNeeded < chosenBlockSize)
  {
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

void removeBlockFromList(int pos, unsigned char *heap)
{
  int next = getNextPtr(pos, heap);
  int prev = getPrevPtr(pos, heap);
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

void malloc_updatePtrs(int pos, bool blockWasSplit, unsigned char *heap)
{
  int next = getNextPtr(pos, heap);
  int prev = getPrevPtr(pos, heap);
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
    removeBlockFromList(pos, heap);
}

// BUG:
// the neighboring blocks can be fragment block which can have neighbors in the free list
// in the current setup, coalescing will stop at the fragment block
// SOLNS:
// 1: change the way fragments are handled (treat them as extra padding)
// 2: if fragment block is the neighbor, keep checking allocated block or free list is found as neighbor

// BUG:
// fragment block will exist without being coalesced to free list: [Allocated][Fragment][Free]
// SOLNS:
// 1: on the creation of fragment block, check if neighboring block is in free list and coalesce
// 2: treat fragments as extra padding: [[Allocated][Fragment]][Free]

// splices blocks and removes spliced free blocks from the free list
// returns to the pos of the newly created free block
int coalesce(int pos, unsigned char *heap)
{
  setSizeHeader(pos, getBlockSize(pos, heap), false, heap);
  setFooter(pos, getBlockSize(pos, heap), false, heap);

  int newPos = pos;
  // check if block after is allocated
  int nbrNext = pos + getBlockSize(pos, heap);
  if (nbrNext < MEMORY_SIZE && !getIsAllocated(nbrNext, heap))
  {
    // remove neighbor from free list if it is in free list
    if (getBlockSize(nbrNext, heap) >= 16)
      removeBlockFromList(nbrNext, heap);
    // merge block
    int newSize = getBlockSize(pos, heap) + getBlockSize(nbrNext, heap);
    setSizeHeader(pos, newSize, false, heap);
    setFooter(pos, newSize, false, heap);
  }
  // check if block before is allocated
  if (0 < pos - FOOTER_SIZE && !getIsAllocated(pos - FOOTER_SIZE, heap))
  {
    int nbrPrev = pos - getBlockSize(pos - FOOTER_SIZE, heap);
    // remove neighbor from free list ifit is in free list
    if (getBlockSize(nbrPrev, heap) >= 16)
      removeBlockFromList(nbrPrev, heap);
    // merge block
    newPos = nbrPrev;
    int newSize = getBlockSize(nbrPrev, heap) + getBlockSize(pos, heap);
    setSizeHeader(nbrPrev, newSize, false, heap);
    setFooter(nbrPrev, newSize, false, heap);
  }
  return newPos;
}

// inserts a block to the beginning of free list
void pushBlock(int pos, unsigned char *heap)
{
  if (pos < 0 || pos >= MEMORY_SIZE)
    return;
  if (root == NULL_PTR)
    setNextPtr(pos, NULL_PTR, heap);
  else
  {
    setNextPtr(pos, root, heap);
    setPrevPtr(root, pos, heap);
  }
  root = pos;
  setPrevPtr(pos, NULL_PTR, heap);
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
    int p = root, best_p = NULL_PTR;
    while (p != NULL_PTR)
    {
      int size = getBlockSize(p, heap);
      best_p = size >= spaceNeeded && size < best_p ? size : best_p;
      p = getNextPtr(p, heap);
    }
    if (best_p == NULL_PTR)
      return NULL;
  }
  else
    return NULL;
  int chosenBlockSize = getBlockSize(p, heap);
  int update_p = splitBlock(p, spaceNeeded, chosenBlockSize, heap);
  bool blockWasSplit = update_p != p ? true : false;
  malloc_updatePtrs(update_p, blockWasSplit, heap);
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
void myfree(void *ptr)
{
  if (ptr == NULL)
    return;
  int pos = (unsigned char *)ptr - heap;
  // coalesce block and update pointers of blocks around the splice
  int newPos = coalesce(pos - SIZE_HEADER, heap);
  // add newly free block to front of list
  pushBlock(newPos, heap);
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
