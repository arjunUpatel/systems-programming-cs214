#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

int main()
{
  myinit(0);
  mymalloc(1);
  mymalloc(1);
  mymalloc(1);
  mymalloc(1);
  return EXIT_SUCCESS;
}