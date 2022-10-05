#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if(argc == 1) {
        printf("Error. No command line input\n");
        return 0;
    }

    long long val = atoll(argv[1]);
    if(val <= 0) {
        printf("enter valid number (positive integer)\n");
        return 0;
    }

    for(long long j = 3; j <= val; j+=2) {
        while((val % j) == 0) {
            printf("%ld ", j);
            val /= j;
        }
    }

    printf("\n");
    return 0;
}