#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    long val = atol(argv[1]);
    for(long j = 2; j <= val; j++) {
        while(!(val % j)) {
            printf("%ld ", j);
            val /= j;
        }
    }
    printf("\n");
    return 0;
}