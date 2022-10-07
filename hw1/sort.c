#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// case insensitive comparing
int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++)
    {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

void freeMainArr(char ***arrPtr, long long size)
{
    if (arrPtr == NULL)
        return;
    char **arr = *arrPtr;
    for (long long i = 0; i < size; i++)
        free(arr[i]);
    free(arr);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        int breakpt = 0;
        long long index = 0;
        long long sizeMain = 1;
        char **input = malloc(sizeof(char *));
        while (1)
        {
            long long size = 2;
            long long ind = 0;
            char *o = (char *)malloc(size);
            while (1)
            {
                if (scanf("%c", &o[ind]) == EOF)
                {
                    breakpt = 1;
                    break;
                }
                if (o[ind] == '\n')
                {
                    o[ind] = '\0';
                    break;
                }

                ind++;

                if (ind == size)
                {
                    char *temp = (char *)malloc(2 * size);
                    for (long long i = 0; i < size; i++)
                        temp[i] = o[i];
                    free(o);
                    o = temp;
                    size *= 2;
                }
            }
            if (breakpt)
            {
                free(o);
                break;
            }
            input[index++] = o;
            if (index == sizeMain)
            {
                char **temp = (char **)malloc(2 * sizeMain * sizeof(char *));
                for (long long i = 0; i < sizeMain; i++)
                    temp[i] = input[i];
                free(input);
                input = temp;
                sizeMain *= 2;
            }
        }
        long long i;
        long long f;
        for (i = 0; i < index - 1; i += 1)
        {
            for (f = 0; f < index - i - 1; f += 1)
            {
                if (strcicmp(input[f], input[f + 1]) > 0)
                {
                    char *temp = input[f];
                    input[f] = input[f + 1];
                    input[f + 1] = temp;
                }
            }
        }
        printf("^D\n");
        for (long long i = 0; i < index; i += 1)
        {
            printf("%s\n", input[i]);
            free(input[i]);
        }
        free(input);
    }
    else if (argc == 2 && strcmp(argv[1], "-n") == 0)
    {
        long long breakpt = 0;
        long long index = 0;
        long long sizeMain = 1;
        char **input = malloc(sizeof(char *));
        while (1)
        {
            long long size = 2;
            long long ind = 0;
            char *o = (char *)malloc(size);
            while (1)
            {
                if (scanf("%c", &o[ind]) == EOF)
                {
                    breakpt = 1;
                    break;
                }
                if (o[ind] == '\n')
                {
                    o[ind] = '\0';
                    break;
                }

                ind++;

                if (ind == size)
                {
                    char *temp = (char *)malloc(2 * size);
                    for (long long i = 0; i < size; i++)
                        temp[i] = o[i];
                    free(o);
                    o = temp;
                    size *= 2;
                }
            }
            if (breakpt)
            {
                free(o);
                break;
            }
            input[index++] = o;
            if (index == sizeMain)
            {
                char **temp = (char **)malloc(2 * sizeMain * sizeof(char *));
                for (long long i = 0; i < sizeMain; i++)
                    temp[i] = input[i];
                free(input);
                input = temp;
                sizeMain *= 2;
            }
        }
        long long *balance = malloc(index * sizeof(long long));
        for (long long t = 0; t < index; t += 1)
        {
            long long i;
            sscanf(input[t], "%d", &i);
            balance[t] = i;
        }
        long long i;
        long long f;
        for (i = 0; i < index - 1; i += 1)
        {
            for (f = 0; f < index - i - 1; f += 1)
            {
                if (balance[f] > balance[f + 1])
                {
                    long long temp = balance[f];
                    balance[f] = balance[f + 1];
                    balance[f + 1] = temp;
                }
            }
        }
        printf("^D\n");
        for (long long i = 0; i < index; i += 1)
        {
            printf("%d\n", balance[i]);
            free(input[i]);
        }
        free(balance);
        free(input);
    }
    else
    {
        printf("Invalid Input\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}