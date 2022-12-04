#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main()
{
  myinit(0);
  char *p = mymalloc(1);
  printf("Malloc pointer: %p\n", p);
  return EXIT_SUCCESS;
}