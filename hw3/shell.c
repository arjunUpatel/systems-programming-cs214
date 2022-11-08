#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

// correct format questions
    // how many ampersands allowed in input?

struct InputParse
{
    char **parsedInput;
    bool ampersandPresent;
};

typedef struct InputParse InputParse;

InputParse *parseInput(char *input)
{
    unsigned long long len = 1;
    unsigned long long idx = 0;
    char **parsedInputTemp = malloc(len * sizeof(char *));
    unsigned long long inputLen = strlen(input);
    bool ampersandPresent = false;

    for (unsigned long long i = 0; i < inputLen; i++)
    {
        if (input[i] == ' ')
            continue;
        if (!ampersandPresent && input[i] == '&')
        {
            ampersandPresent = true;
            continue;
        }

        unsigned long long strLen = 1;
        unsigned long long strIdx = 0;
        char *strTemp = malloc(strLen * sizeof(char));
        unsigned long long j = i;

        while (j < inputLen && input[j] != ' ')
        {
            strTemp[strIdx] = input[j];
            strIdx++;
            if (strIdx == strLen)
            {
                char *temp = malloc(2 * strLen * sizeof(char));
                for (unsigned long long k = 0; k < strLen; k++)
                    temp[k] = strTemp[k];
                free(strTemp);
                strTemp = temp;
                strLen *= 2;
            }
            j++;
        }
        strTemp[strIdx] = '\0';
        strIdx++;
        char *str = malloc(strIdx * sizeof(char));
        for (unsigned long long l = 0; l < strIdx; l++)
            str[l] = strTemp[l];
        free(strTemp);
        parsedInputTemp[idx] = str;
        idx++;
        if (idx == len)
        {
            char **temp = malloc(2 * len * sizeof(char *));
            for (unsigned long long l = 0; l < idx; l++)
                temp[l] = parsedInputTemp[l];
            free(parsedInputTemp);
            parsedInputTemp = temp;
            len *= 2;
        }
        i = j + 1;
    }
    parsedInputTemp[idx] = NULL;
    idx++;
    char **parsedInput = malloc(idx * sizeof(char *));
    for (unsigned long long l = 0; l < idx; l++)
        parsedInput[l] = parsedInputTemp[l];
    free(parsedInputTemp);

    InputParse *inputParse = malloc(sizeof(InputParse));
    inputParse->parsedInput = parsedInput;
    inputParse->ampersandPresent = ampersandPresent;

    return inputParse;
}

int main()
{
    while (1)
    {
        long long bufLen = 1;
        long long idx = 0;
        char *buf = malloc(sizeof(char));
        while (!feof(stdin))
        {
            char c;
            scanf("%c", c);
            buf[idx] = c;
            idx++;
            if (idx == bufLen)
            {
                char *temp = malloc(2 * bufLen * sizeof(char));
                for (int i = 0; i < bufLen; i++)
                    temp[i] = buf[i];
                free(buf);
                buf = temp;
                bufLen *= 2;
            }
        }
        parser(buf);
        printf("> ");
        break;
    }
}
