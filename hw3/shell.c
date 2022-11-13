#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "parser.h"
#include "process.h"

// correct format questions
// how many ampersands allowed in input?
// will ampersand only be placed at the end

pid_t foregroundPID = -1;

void handleSigint(int signum)
{
  if (foregroundPID > -1)
  {
    kill(foregroundPID, SIGINT);
    foregroundPID = -1;
  }
}

void handleSigtstp(int signum)
{
  if (foregroundPID > -1)
  {
    kill(foregroundPID, SIGTSTP);
    foregroundPID = -1;
  }
}

int main()
{
  Process **jobs = calloc(5, sizeof(Process *));
  int numJobs = 0;

  signal(SIGINT, handleSigint);
  signal(SIGTSTP, handleSigtstp);

  while (1)
  {
    printf("> ");
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

    // printf("input: %s\n", buf);

    InputParse *parsedInput = parseInput(buf);
    createProcess(parsedInput, jobs, numJobs);

    // printParsedInput(parsedInput);
    freeInputParse(parsedInput);
    free(buf);
  }
}
