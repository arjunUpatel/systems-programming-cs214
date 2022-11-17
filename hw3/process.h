#pragma once

#include <sys/types.h>
#include "parser.h"
#include "stack.h"

typedef struct Stack Stack;

typedef struct Process
{
  int jid;
  pid_t pid;
  // 0 = running, 1 = stopped, 2 = terminated
  int status;
  InputParse *inputParse;
} Process;

void createProcess(InputParse *inputParse, Stack *jobStack, pid_t shell_pid);
void printJob(Process *process);
void freeProcess(Process *process);
