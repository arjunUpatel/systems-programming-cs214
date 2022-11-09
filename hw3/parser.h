#include <stdbool.h>

struct InputParse
{
    char **parsedInput;
    bool ampersandPresent;
};
typedef struct InputParse InputParse;

InputParse *parseInput(char *input);
void freeInputParse(InputParse *inputParse);
void printParsedInput(InputParse *inputParse);
