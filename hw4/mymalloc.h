#include <stdlib.h>

void printHeap();
void myinit(int allocAlg);
void *mymalloc(size_t size);
void myfree(void *ptr);
void *myrealloc(void *ptr, size_t size);
void mycleanup();
