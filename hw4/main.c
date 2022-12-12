#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main()
{
  myinit(0);
  void *p1 = mymalloc(1);
  printHeap();
  void *p2 = mymalloc(1);
  printHeap();
  myfree(p1);
  printHeap();
  myfree(p2);
  printHeap();
  return EXIT_SUCCESS;
}