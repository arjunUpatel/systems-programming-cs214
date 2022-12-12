#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main()
{
  myinit(0);
  void *p1 = mymalloc(1);
  printf("Malloc 1\n");
  printHeap();
  void *p2 = mymalloc(1);
  printf("\nMalloc 1\n");
  printHeap();
  void *p3 = myrealloc(p1, 9);
  printf("\nRealloc first block to 9\n");
  printHeap();
  myfree(p2);
  printf("\nFree second block\n");
  printHeap();
  void *p4 = mymalloc(1);
  printf("\nMalloc 1\n");
  printHeap();
  mycleanup();
  printf("\nClean up\n");
  return EXIT_SUCCESS;
}