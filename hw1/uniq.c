#include <stdio.h>
#include <stdlib.h>

int main()
{
    char **m = (char **)malloc(sizeof(char *));
    long long *ct = (long long *)malloc(sizeof(long long));

    long long len = 0;
    long long Max_Len = 1;

    int breakpt = 0;
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
                {
                    temp[i] = o[i];
                }
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

        int indicator = 0;
        if (len != 0)
        {
            long long i = len - 1;
            long long tf = 1;
            long long ind = 0;
            while ((m[i][ind] != '\0') && (o[ind] != '\0'))
            {
                if (m[i][ind] != o[ind])
                {
                    tf = 0;
                    break;
                }
                ind++;
            }
            if ((m[i][ind] == '\0') ^ (o[ind] == '\0'))
                tf = 0;

            if (tf)
            {
                ct[i]++;
                indicator = 1;
            }
        }

        if (!indicator)
        {
            m[len] = o;
            ct[len] = 1;
            len++;

            if (len >= Max_Len)
            {
                Max_Len *= 2;
                char **tempArr = (char **)malloc((Max_Len) * sizeof(char *));
                long long *tempCt = (long long *)malloc((Max_Len) * sizeof(long long));

                for (long long i = 0; i < len; i++)
                {
                    tempArr[i] = m[i];
                    tempCt[i] = ct[i];
                }

                free(ct);
                free(m);
                ct = tempCt;
                m = tempArr;
            }
        }
        else
            free(o);
    }

    for (long long i = 0; i < len; i++)
    {
        printf("%lld %s\n", ct[i], m[i]);
    }

    for (long long i = 0; i < len; i++)
    {
        free(m[i]);
    }
    free(m);
    free(ct);

    return 0;
}
