#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "parser.h"
#include "stack.h"
#include "process.h"

void handle_sigint(int signum)
{
  write(STDOUT_FILENO, "\n> ", 3);
}

int main()
{
  signal(SIGINT, handle_sigint);
  signal(SIGTSTP, SIG_IGN);
  pid_t shell_pid = getpid();
  tcsetpgrp(STDIN_FILENO, shell_pid);
  Stack *jobStack = malloc(sizeof(Stack));
  jobStack->head = NULL;

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
        breakpt = true;
        break;
      }
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
      exitShell(jobStack);
      break;
    }

    char *buf = malloc(idx * sizeof(char));
    for (int i = 0; i < idx; i++)
      buf[i] = bufTemp[i];
    free(bufTemp);
    InputParse *parsedInput = parseInput(buf);
    if (parsedInput != NULL)
    {
      createProcess(parsedInput, jobStack, shell_pid);
    }
    free(buf);
  }
  freeStack(jobStack);
}
