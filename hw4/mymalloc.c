#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

static unsigned char *heap = NULL;
static int alg = -1;
static int root = -1;
static char *searchPtr = NULL;

const int HEADER_SIZE = 12;
const int BLOCK_SIZE = 4;
const int NEXT_PTR = 4;
const int PREV_PTR = 4;
const int FOOTER_SIZE = 4;
const int MEMORY_SIZE = 48; // 1024 * 1024;
const int ALIGNMENT = 8;
const int NULL_POINTER = 0;

void printHeap()
{
  printf("Heap: ");
  for (int i = 0; i < MEMORY_SIZE; i++)
  {
    printf("%d ", heap[i]);
  }
  printf("\n");
}

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

int getBlockSize(int pos, unsigned char *heap)
{
  return readInt(pos - HEADER_SIZE, heap);
}

int getNextPtr(int pos, unsigned char *heap)
{
  return readInt(pos - HEADER_SIZE + BLOCK_SIZE, heap);
}

int getPrevPtr(int pos, unsigned char *heap)
{
  return readInt(pos - HEADER_SIZE + BLOCK_SIZE + NEXT_PTR, heap);
}

void setHeaderSize(int pos, int size, unsigned char *heap)
{
  insertInt(size, pos - HEADER_SIZE, heap);
}

void setNextPtr(int pos, int newNext, unsigned char *heap)
{
  insertInt(newNext, pos - HEADER_SIZE + BLOCK_SIZE, heap);
}

void setPrevPtr(int pos, int newPrev, unsigned char *heap)
{
  insertInt(newPrev, pos - HEADER_SIZE + BLOCK_SIZE + NEXT_PTR, heap);
}

void setFooterSize(int pos, int size, unsigned char *heap)
{
  insertInt(size, pos + size - HEADER_SIZE - FOOTER_SIZE, heap);
}

// Inserts size and isAllocated data
int insertSize(int size, bool isAllocated, int insertPos, unsigned char *heap)
{
  int value = size << 1;
  if (isAllocated)
    value += 1;
  return insertInt(value, insertPos, heap);
}

// Gets the size of a block
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

// Gets value of isAllocated bit
bool readIsAllocated(int pos, unsigned char *heap)
{
  return heap[pos + 3] & 1;
}

void allocateBlock(int pos, int payloadSize, unsigned char *heap)
{
  // TODO: Round up to nearest 8
  int size = 4 + payloadSize + 4;
  // Insert header
  setHeaderSize(pos, size, heap);
  // Fill payload with 1s
  for (int i = pos + 4; i < pos + 4 + payloadSize; i++)
  {
    heap[i] = 1;
  }
  // Insert footer
  setFooterSize(pos, size, heap);
}

void addFreeBlock(int pos, int size, int next, int prev, unsigned char *heap)
{
  // Ensure block size can fit header, pointers, and footer
  if (size >= HEADER_SIZE + FOOTER_SIZE)
  {
    setHeaderSize(pos, size, heap);
    setNextPtr(pos, next, heap);
    setPrevPtr(pos, prev, heap);
    setFooterSize(pos, size, heap);
  }
}

void removeFreeBlock(int pos, unsigned char *heap)
{
  int next = getNextPtr(pos, heap);
  int prev = getPrevPtr(pos, heap);

  if (prev != NULL_POINTER && next != NULL_POINTER)
  {
    // Prev and next pointers exist
    setNextPtr(prev, next, heap);
    setPrevPtr(next, prev, heap);
  }
  else if (prev == NULL_POINTER && next != NULL_POINTER)
  {
    // Prev pointer is null
    setPrevPtr(next, NULL_POINTER, heap);
  }
  else if (prev != NULL_POINTER && next == NULL_POINTER)
  {
    // Next pointer is null
    setNextPtr(prev, NULL_POINTER, heap);
  }
}

int splitFreeBlock(int pos, int spaceNeeded, int freeBlockSize, unsigned char *heap)
{
  int splitSize = freeBlockSize - spaceNeeded;
  int splitPos = pos + spaceNeeded;
  // copy next ptr into split
  setNextPtr(splitPos, getNextPtr(pos, heap), heap);
  // copy prev ptr into split
  setPrevPtr(splitPos, getPrevPtr(pos, heap), heap);
  // change size of split block in header
  setHeaderSize(splitPos, splitSize, heap);
  // change size of split block in footer
  setFooterSize(splitPos, splitSize, heap);
  // change size of chosen block in header
  setHeaderSize(pos, spaceNeeded, heap);
  // update size of chosen block in footer
  setFooterSize(pos, spaceNeeded, heap);
  return splitPos;
}

void myinit(int allocAlg)
{
  heap = calloc(sizeof(unsigned char), MEMORY_SIZE);

  setHeaderSize(HEADER_SIZE, MEMORY_SIZE, heap);
  setNextPtr(HEADER_SIZE, NULL_POINTER, heap);
  setPrevPtr(HEADER_SIZE, NULL_POINTER, heap);
  setFooterSize(HEADER_SIZE, MEMORY_SIZE, heap);

  printHeap();
  root = HEADER_SIZE;
  alg = allocAlg;

  printf("Read pos 0: %d\n", readInt(0, heap));
  printf("Read pos 4: %d\n", readInt(4, heap));
  printf("Read pos 8: %d\n", readInt(8, heap));
  // printf("Read pos 12: %d\n", readSize(12, heap));
  // printf("Read pos 12 allocated: %d\n", readIsAllocated(12, heap));
  printf("Read pos 16: %d\n", readInt(16, heap));
  printf("Read pos 20: %d\n", readInt(20, heap));
  printf("Read pos 24: %d\n", readInt(24, heap));
}

void *mymalloc(size_t size)
{
  if (size == 0 || root < 0)
    return NULL;

  int blockSize = size + BLOCK_SIZE + FOOTER_SIZE;
  int pos = root;
  while (1)
  {
    int freeBlockSize = getBlockSize(pos, heap);
    if (freeBlockSize < blockSize)
    {
      // Jump to next free block if size is too small
      pos = getNextPtr(pos, heap);
      if (pos == NULL_POINTER)
        return NULL;
    }
    else
    {
      // When space is found, allocate block and split free block
      int next = getNextPtr(pos, heap);
      int prev = getPrevPtr(pos, heap);
      splitFreeBlock(pos, blockSize, freeBlockSize, heap);

      if (freeBlockSize >= blockSize + HEADER_SIZE + FOOTER_SIZE)
      {
        if (prev == NULL_POINTER)
          root = pos + blockSize;
      }
      else
      {
        if (pos == root)
        {
          if (next != NULL_POINTER)
            root = next;
          else
            root = -1;
        }
      }

      printf("Malloced ");
      printHeap();
      return &heap[pos];
    }
  }

  return NULL;
}

// Converting void pointers to char
// void *p = &heap[pos];
// unsigned char c = *((unsigned char *)p);
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