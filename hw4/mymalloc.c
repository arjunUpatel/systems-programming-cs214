#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

static unsigned char *heap = NULL;
static int alg = -1;
static int root = -1;
static char *searchPtr = NULL;

const int FREE_HEADER_SIZE = 12;
const int SIZE_HEADER = 4;
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
  for (int i = 24; i >= 0; i -= 8)
  {
    result += heap[pos] * pow(2.0, i);
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

  // Shift result due to masking out last bit
  return result >> 1;
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
  int value = size << 1;
  if (isAllocated)
    value += 1;
  return insertInt(value, pos, heap);
}

int setFooter(int pos, int size, bool isAllocated, unsigned char *heap)
{
  int value = size << 1;
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

// void allocateBlock(int pos, int payloadSize, unsigned char *heap)
// {
//   // TODO: Round up to nearest 8
//   int size = 4 + payloadSize + 4;
//   // Insert header
//   setHeaderSize(pos, size, heap);
//   // Fill payload with 1s
//   for (int i = pos + 4; i < pos + 4 + payloadSize; i++)
//   {
//     heap[i] = 1;
//   }
//   // Insert footer
//   setFooterSize(pos, size, heap);
// }

// void addFreeBlock(int pos, int size, int next, int prev, unsigned char *heap)
// {
//   // Ensure block size can fit header, pointers, and footer
//   if (size >= FREE_HEADER_SIZE + FOOTER_SIZE)
//   {
//     setHeaderSize(pos, size, heap);
//     setNextPtr(pos, next, heap);
//     setPrevPtr(pos, prev, heap);
//     setFooterSize(pos, size, heap);
//   }
// }

// void removeFreeBlock(int pos, unsigned char *heap)
// {
//   int next = getNextPtr(pos, heap);
//   int prev = getPrevPtr(pos, heap);

//   if (prev != NULL_POINTER && next != NULL_POINTER)
//   {
//     // Prev and next pointers exist
//     setNextPtr(prev, next, heap);
//     setPrevPtr(next, prev, heap);
//   }
//   else if (prev == NULL_POINTER && next != NULL_POINTER)
//   {
//     // Prev pointer is null
//     setPrevPtr(next, NULL_POINTER, heap);
//   }
//   else if (prev != NULL_POINTER && next == NULL_POINTER)
//   {
//     // Next pointer is null
//     setNextPtr(prev, NULL_POINTER, heap);
//   }
// }

int splitFreeBlock(int pos, int spaceNeeded, int freeBlockSize, unsigned char *heap)
{
  int splitSize = freeBlockSize - spaceNeeded;
  int splitPos = pos + spaceNeeded;
  // Copy next ptr into split
  setNextPtr(splitPos, getNextPtr(pos, heap), heap);
  // Copy prev ptr into split
  setPrevPtr(splitPos, getPrevPtr(pos, heap), heap);
  // Change size of split block in header
  setSizeHeader(splitPos, splitSize, false, heap);
  // Change size of split block in footer
  setFooter(splitPos, splitSize, false, heap);
  // Change size of chosen block in header
  setSizeHeader(pos, spaceNeeded, true, heap);
  // Update size of chosen block in footer
  setFooter(pos, spaceNeeded, true, heap);
  return splitPos;
}

void myinit(int allocAlg)
{
  heap = calloc(sizeof(unsigned char), MEMORY_SIZE);

  setSizeHeader(0, MEMORY_SIZE, false, heap);
  setNextPtr(0, NULL_POINTER, heap);
  setPrevPtr(0, NULL_POINTER, heap);
  setFooter(0, MEMORY_SIZE, false, heap);

  printHeap();
  root = 0;
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

  int blockSize = size + SIZE_HEADER + FOOTER_SIZE;
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

      // Make sure root points to first free block
      if (freeBlockSize >= blockSize + FREE_HEADER_SIZE + FOOTER_SIZE)
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
void myfree(void *ptr)
{
  if (ptr == NULL)
    return;

  int pos = (unsigned char *)ptr - heap;
  int size = getBlockSize(pos, heap);

  if (size >= FREE_HEADER_SIZE + FOOTER_SIZE)
  {
    // setHeaderSize(pos, size, heap);
    // setNextPtr(pos, next, heap);
    // setPrevPtr(pos, prev, heap);
    // setFooterSize(pos, size, heap);
  }
}

void *myrealloc(void *ptr, size_t size);

void mycleanup()
{
  free(heap);
  heap = NULL;
  root = -1;
  searchPtr = NULL;
  alg = -1;
}