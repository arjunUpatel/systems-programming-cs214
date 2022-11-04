#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int curJobID = 0;
pid_t foregroundPID = -1;

void pathMapper(int isBackground)
{
  curJobID++;
  pid_t pid = 0;
  int jobid = curJobID;
  int status;

  pid = fork();
  foregroundPID = pid;

  if (pid == 0)
  {
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    char *path = "./hi";
    if (access(path, X_OK) == 0)
    {
      sleep(3);
      char *argv[] = {path, NULL};
      execv(argv[0], argv);
      exit(0);
    }
    else
    {
      printf("%s: No such file or directory\n", path);
      exit(1);
    }
  }
  if (pid > 0)
  {
    if (isBackground)
    {
      printf("[%d] %d\n", jobid, pid);
      foregroundPID = -1;
    }
    else
    {
      pid = waitpid(pid, &status, WUNTRACED);
      foregroundPID = -1;
      printf("End of process %d, job id %d: ", pid, jobid);
      if (WIFEXITED(status))
      {
        printf("The process ended with exit(%d).\n", WEXITSTATUS(status));
      }
      if (WIFSIGNALED(status))
      {
        printf("The process ended with kill (%d).\n", WTERMSIG(status));
      }
    }
  }
}

void handleSigint(int signum)
{
  if (foregroundPID > -1)
  {
    kill(foregroundPID, SIGINT);
  }
}

void handleSigtstp(int signum)
{
  if (foregroundPID > -1)
  {
    kill(foregroundPID, SIGTSTP);
  }
}

int main()
{
  signal(SIGINT, handleSigint);
  signal(SIGTSTP, handleSigtstp);

  while (1)
  {
    char str[20];
    printf("> ");
    scanf("%19s", str);
    pathMapper(0);
  }
}