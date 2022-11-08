#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct Job
{
  int jobid;
  pid_t pid;
  // 0 = running, 1 = stopped, 2 = terminated
  int status;
  int inBackground;
  char *command;
} Job;

int maxJobs = 5;
int numJobs = 0;
pid_t foregroundPID = -1;

char *statusToString(int status)
{
  switch (status)
  {
  case 0:
    return "Running";
  case 1:
    return "Stopped";
  case 2:
    return "Terminated";
  default:
    return "";
  }
}

void printJobs(Job **jobs)
{
  for (int i = 0; i < numJobs; i++)
  {
    printf("[%d] %d %s %s\n", jobs[i]->jobid, jobs[i]->pid, statusToString(jobs[i]->status), jobs[i]->command);
  }
}

void addJob(Job **jobs, pid_t pid, int jobid, int inBackground, char *cmd)
{
  if (numJobs > maxJobs)
  {
    maxJobs *= 2;
    jobs = realloc(jobs, maxJobs * sizeof(Job *));
  }

  jobs[numJobs - 1] = malloc(sizeof(Job) + strlen(cmd) + 1);
  jobs[numJobs - 1]->pid = pid;
  jobs[numJobs - 1]->jobid = jobid;
  jobs[numJobs - 1]->status = 0;
  jobs[numJobs - 1]->inBackground = inBackground;
  jobs[numJobs - 1]->command = cmd;
}

void createProcess(char **args, int inBackground, Job **jobs)
{
  numJobs++;
  pid_t pid = 0;
  int jobid = numJobs;
  int status;

  pid = fork();
  foregroundPID = pid;

  if (pid == 0)
  {
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);

    sleep(3);
    execv(args[0], args);
    exit(0);
  }
  if (pid > 0)
  {
    addJob(jobs, pid, jobid, inBackground, args[0]);

    if (inBackground)
    {
      printf("[%d] %d\n", jobid, pid);
      foregroundPID = -1;
    }
    else
    {
      pid = waitpid(pid, &status, WUNTRACED);
      foregroundPID = -1;
      if (WIFSIGNALED(status))
      {
        printf("\n[%d] %d terminated by signal %d\n", jobid, pid, WTERMSIG(status));
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

  Job **jobs = calloc(maxJobs, sizeof(Job *));

  while (1)
  {
    char str[20];
    printf("> ");
    scanf("%19s", str);

    char *args[] = {"./hi", NULL};
    createProcess(args, 0, jobs);
    printJobs(jobs);
  }
}