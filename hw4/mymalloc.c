#include <stdlib.h>

static char *heap = NULL;
static int alg = -1;

void myinit(int allocAlg)
{
    heap = calloc(sizeof(char), 1024 * 1024);
    alg = allocAlg;
}

void *mymalloc(size_t size);
void myfree(void *ptr);
void *myrealloc(void *ptr, size_t size);

void mycleanup()
{
    free(heap);
    heap = NULL;
    alg = -1;
}
