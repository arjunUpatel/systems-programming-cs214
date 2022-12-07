#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main()
{
  myinit(0);
  void *p1 = mymalloc(1);
  myfree(p1);
  void *p2 = mymalloc(1);
  return EXIT_SUCCESS;
}