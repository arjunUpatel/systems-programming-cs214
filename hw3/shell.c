#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

// correct format questions
// how many ampersands allowed in input?
// will ampersand only be placed at the end

int main()
{
  while (1)
  {
    long long bufLen = 1;
    long long idx = 0;
    char *bufTemp = malloc(sizeof(char));
    bool breakpt = false;
    while (1)
    {
      char c;
      if (scanf("%c", &c) == EOF)
      {
        // handle ctrl + z placeholder
        breakpt = true;
        break;
      }
      // else if(scanf("%c", &c) == ETX)
      // {
      // handle ctrl + c placeholder
      // }
      if (c == '\n')
      {
        bufTemp[idx] = '\0';
        idx++;
        break;
      }

      bufTemp[idx] = c;
      idx++;
      if (idx == bufLen)
      {
        char *temp = malloc(2 * bufLen * sizeof(char));
        for (int i = 0; i < bufLen; i++)
          temp[i] = bufTemp[i];
        free(bufTemp);
        bufTemp = temp;
        bufLen *= 2;
      }
    }
    if (breakpt)
    {
      free(bufTemp);
      break;
    }

    char *buf = malloc(idx * sizeof(char));
    for (int i = 0; i < idx; i++)
      buf[i] = bufTemp[i];
    free(bufTemp);

    printf("input: %s\n", buf);
    InputParse *parsedInput = parseInput(buf);
    printParsedInput(parsedInput);
    freeInputParse(parsedInput);
    free(buf);
  }
}
