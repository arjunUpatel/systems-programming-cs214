#include <sys/types.h>
#include "parser.h"

typedef struct Process
{
  int jid;
  pid_t pid;
  // 0 = running, 1 = stopped, 2 = terminated
  int status;
  InputParse *inputParse;
} Process;

void createProcess(InputParse *inputParse, Process **jobs, int numJobs);
void printJob(Process *process);
void freeProcess(Process *process);
