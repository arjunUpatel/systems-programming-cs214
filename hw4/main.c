#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

// BUG: best fit not reaching allocation part

int main()
{
  printf("test for proper coalescing\n");
  myinit(0);
  void *a1 = mymalloc(1);
  void *a2 = mymalloc(1);
  void *a3 = mymalloc(1);
  void *a4 = mymalloc(1);
  void *a5 = mymalloc(1);
  myfree(a1);
  myfree(a3);
  myfree(a5);
  myfree(a2);
  myfree(a4);
  printHeap();
  mycleanup();

  printf("\ntest for proper internal fragmentation\n");
  myinit(0);
  mymalloc(57);
  printHeap();
  mycleanup();

  printf("\ntest for proper pointers after free\n");
  myinit(1);
  void *b1 = mymalloc(1);
  void *b2 = mymalloc(1);
  void *b3 = mymalloc(1);
  void *b4 = mymalloc(1);
  myfree(b1);
  myfree(b4);
  printHeap();
  mycleanup();

  printf("\ntest for proper pointers after split\n");
  myinit(0);
  void *c1 = mymalloc(17);
  void *c2 = mymalloc(1);
  void *c3 = mymalloc(1);
  void *c4 = mymalloc(1);
    myfree(c3);
  myfree(c1);
  // myfree(c1);
  // myfree(c4);
  mymalloc(1);
  printHeap();
  mycleanup();

  printf("\nmalloc w/ alg = 1\n");
  myinit(1);
  printf("Malloc 1\n");
  void *p1 = mymalloc(1);
  printHeap();
  printf("\nMalloc 1\n");
  void *p2 = mymalloc(1);
  printHeap();
  printf("\nRealloc first block to 9\n");
  void *p3 = myrealloc(p1, 9);
  printHeap();
  printf("\nFree second block\n");
  myfree(p2);
  printHeap();
  printf("\nMalloc 1\n");
  void *p4 = mymalloc(1);
  printHeap();
  printf("\nClean up\n");
  mycleanup();

  printf("malloc w/ alg = 0\n");
  myinit(0);
  void *p5 = mymalloc(1);
  printf("Malloc 1\n");
  printHeap();
  void *p6 = mymalloc(1);
  printf("\nMalloc 1\n");
  printHeap();
  void *p7 = myrealloc(p5, 9);
  printf("\nRealloc first block to 9\n");
  printHeap();
  myfree(p6);
  printf("\nFree second block\n");
  printHeap();
  void *p8 = mymalloc(1);
  printf("\nMalloc 1\n");
  printHeap();
  return EXIT_SUCCESS;
}