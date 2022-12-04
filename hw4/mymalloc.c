#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

static unsigned char *heap = NULL;
static int alg = -1;
static int root = -1;
static char *searchPtr = NULL;

// Equivalent to (255 255 255 255)
// Converts to -2130706433 when read as an integer
const int NULL_POINTER = 4294967295;
const int HEAP_SIZE = 48;

void printHeap()
{
  printf("Heap: ");
  for (int i = 0; i < HEAP_SIZE; i++)
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
  insertSize(size, true, pos, heap);
  // Fill payload with 1s
  for (int i = pos + 4; i < pos + 4 + payloadSize; i++)
  {
    heap[i] = 1;
  }
  // Insert footer
  insertSize(size, true, pos + 4 + payloadSize, heap);
}

void addFreeBlock(int pos, int blockSize, int next, int prev, unsigned char *heap)
{
  // Ensure block size can fit header, pointers, and footer
  if (blockSize >= 16)
  {
    insertSize(blockSize, false, pos, heap);
    insertInt(next, pos + 4, heap);
    insertInt(prev, pos + 8, heap);
    insertSize(blockSize, false, pos + blockSize - 4, heap);
  }
}

void removeFreeBlock(int pos, unsigned char *heap)
{
  int size = readSize(pos, heap);
  int next = readInt(pos + 4, heap);
  int prev = readInt(pos + 8, heap);
  int footerPos = pos + size - 4;

  if (prev >= 0 && next >= 0)
  {
    // Prev and next pointers exist
    insertInt(next, prev + 4, heap);
    insertInt(prev, next + 8, heap);
  }
  else if (prev < 0 && next >= 0)
  {
    // Prev pointer is null
    insertInt(NULL_POINTER, next + 8, heap);
  }
  else if (prev >= 0 && next < 0)
  {
    // Next pointer is null
    insertInt(NULL_POINTER, prev + 4, heap);
  }

  // Clear out data to 0s
  for (int i = pos; i < pos + 12; i++)
  {
    heap[i] = 0;
  }
  for (int i = footerPos; i < footerPos + 4; i++)
  {
    heap[i] = 0;
  }
}

void myinit(int allocAlg)
{
  heap = calloc(sizeof(unsigned char), HEAP_SIZE);
  int insertPos = 0;

  // insert size in front
  insertPos = insertSize(HEAP_SIZE, false, insertPos, heap);
  // insert next ptr
  insertPos = insertInt(NULL_POINTER, insertPos, heap);
  // insert prev ptr
  insertPos = insertInt(NULL_POINTER, insertPos, heap);
  // insert size in end
  insertSize(HEAP_SIZE, false, HEAP_SIZE - 4, heap);

  printHeap();
  root = 0;
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

void *mymalloc(size_t size)
{
  if (size == 0)
    return NULL;

  int blockSize = size + 8;
  int pos = root;
  while (1)
  {
    int freeBlockSize = readSize(pos, heap);
    if (freeBlockSize < blockSize)
    {
      // Jump to next free block if size is too small
      pos = readInt(pos + 4, heap);
      if (pos < 0)
        return NULL;
    }
    else
    {
      // When space is found, allocate block and return memory address
      // TODO: Test block splitting

      int next = readInt(pos + 4, heap);
      int prev = readInt(pos + 8, heap);
      if (freeBlockSize >= blockSize + 16)
      {
        // Split free block
        removeFreeBlock(pos, heap);
        addFreeBlock(pos + blockSize, freeBlockSize - blockSize, next, prev, heap);
        if (prev < 0)
          root = pos + blockSize;
      }
      else
      {
        // No space to split block
        // TODO: Test
        if (pos == root)
        {
          if (next >= 0)
            root = next;
          else
            root = -1;
        }
        removeFreeBlock(pos, heap);
      }
      allocateBlock(pos, size, heap);

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