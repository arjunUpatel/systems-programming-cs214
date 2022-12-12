#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main()
{
  printf("malloc w/ alg = 1\n");
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