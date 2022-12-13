#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main()
{
  printf("\nmalloc w/ alg = 1\n");
  myinit(0);
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

  return EXIT_SUCCESS;
}