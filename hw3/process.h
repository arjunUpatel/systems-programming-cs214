#include <sys/types.h>

typedef struct Process
{
  int jid;
  pid_t pid;
  // 0 = running, 1 = stopped, 2 = terminated
  int status;
  char *command;
} Process;

void createProcess(InputParse *inputParse, Process **jobs, int numJobs);