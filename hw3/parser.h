#pragma once
#include <stdbool.h>

typedef struct InputParse
{
    char **parsedInput;
    bool ampersandPresent;
    int parseLen;
} InputParse;

InputParse *parseInput(char *input);
void freeInputParse(InputParse *inputParse);
void printParsedInput(InputParse *inputParse);
