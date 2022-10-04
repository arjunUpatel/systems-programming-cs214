#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    char *val;
    int swi = 0;

    if (argv[1][0] == '-' && argv[1][1] == 'i' && argv[1][2] == '\0' && argc == 3)
    {
        swi++;
        val = argv[2];
    }
    else if (argc == 2)
    {
        val = argv[1];
    }
    else
    {
        printf("invalid input\n");
        return 0;
    }

    while (1)
    {
        char *o = (char *)malloc(8195);
        if (scanf("%[^\n]%*c", o) == EOF)
            break;

        int first = 0;
        int indicator = 0;

        while (1)
        {
            int tf = 1;
            int ind = 0;

            while ((o[ind + first] != '\0'))
            {
                if (val[ind] == '\0')
                    break;

                if (swi)
                {
                    int dist = abs(val[ind] - o[ind + first]);

                    if (dist != 0 && dist != 32)
                    {
                        tf = 0;
                        break;
                    }
                }
                else if (val[ind] != o[ind + first])
                {
                    tf = 0;
                    break;
                }

                ind++;
            }

            if (!(val[ind] == '\0') && (o[ind + first] == '\0'))
                tf = 0;
            if (tf)
                indicator = 1;
            if (o[ind + first] == '\0' || indicator)
                break;

            first++;
        }
        if (indicator)
            printf("%s\n", o);

        free(o);
    }

    return 0;
}