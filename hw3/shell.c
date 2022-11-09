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
  int jobID;
  pid_t pid;
  // 0 = running, 1 = stopped, 2 = terminated
  int status;
  int inBackground;
  char *command;
} Job;

Job **jobs;

int maxJobs = 5;
int numJobs = 0;
int foregroundJobID = -1;

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
    printf("[%d] %d %s %s\n", jobs[i]->jobID, jobs[i]->pid, statusToString(jobs[i]->status), jobs[i]->command);
  }
}

void addJob(Job **jobs, pid_t pid, int jobID, int inBackground, char *cmd)
{
  if (numJobs > maxJobs)
  {
    maxJobs *= 2;
    jobs = realloc(jobs, maxJobs * sizeof(Job *));
  }

  jobs[numJobs - 1] = malloc(sizeof(Job) + strlen(cmd) + 1);
  jobs[numJobs - 1]->pid = pid;
  jobs[numJobs - 1]->jobID = jobID;
  jobs[numJobs - 1]->status = 0;
  jobs[numJobs - 1]->inBackground = inBackground;
  jobs[numJobs - 1]->command = cmd;
}

void createProcess(char **args, int inBackground, Job **jobs)
{
  numJobs++;
  pid_t pid = 0;
  int jobID = numJobs;
  int status;

  pid = fork();
  foregroundJobID = jobID;

  if (pid == 0)
  {
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);

    sleep(5);
    execv(args[0], args);
    exit(0);
  }
  if (pid > 0)
  {
    addJob(jobs, pid, jobID, inBackground, args[0]);

    if (inBackground)
    {
      printf("[%d] %d\n", jobID, pid);
      foregroundJobID = -1;
    }
    else
    {
      pid = waitpid(pid, &status, WUNTRACED);
      foregroundJobID = -1;
      if (WIFSIGNALED(status))
      {
        printf("\n[%d] %d terminated by signal %d\n", jobID, pid, WTERMSIG(status));
      }
    }
  }
}

void handleFg(int jobID)
{
  if (jobID <= numJobs)
  {
    Job *job = jobs[jobID - 1];
    if (job->status == 1 || job->inBackground == 1)
    {
      if (job->status == 1)
      {
        kill(job->pid, SIGCONT);
      }
      job->inBackground = 0;
      job->status = 0;
      foregroundJobID = job->jobID;

      int status;
      waitpid(job->pid, &status, WUNTRACED);
      foregroundJobID = -1;
      if (WIFSIGNALED(status))
      {
        printf("\n[%d] %d terminated by signal %d\n", job->jobID, job->pid, WTERMSIG(status));
      }
    }
    else
    {
      printf("Job is not suspended\n");
    }
  }
}

void handleBg(int jobID)
{
  if (jobID <= numJobs)
  {
    Job *job = jobs[jobID - 1];
    if (job->status == 1)
    {
      kill(job->pid, SIGCONT);
      job->status = 0;
    }
    else
    {
      printf("Job is not suspended\n");
    }
  }
}

void handleKill(int jobID)
{
  if (jobID <= numJobs)
  {
    Job *job = jobs[jobID - 1];
    job->status = 2;
    kill(job->pid, SIGTERM);
  }
}

void handleSigint(int signum)
{
  if (foregroundJobID > -1)
  {
    Job *job = jobs[foregroundJobID - 1];
    job->status = 2;
    kill(job->pid, SIGINT);
    foregroundJobID = -1;
  }
}

void handleSigtstp(int signum)
{
  if (foregroundJobID > -1)
  {
    Job *job = jobs[foregroundJobID - 1];
    job->status = 1;
    kill(job->pid, SIGTSTP);
    foregroundJobID = -1;
  }
}

int main()
{
  signal(SIGINT, handleSigint);
  signal(SIGTSTP, handleSigtstp);

  jobs = calloc(maxJobs, sizeof(Job *));

  while (1)
  {
    char str[20];
    printf("> ");
    scanf("%19s", str);

    if (strcmp(str, "bg") == 0)
    {
      handleBg(1);
    }
    else if (strcmp(str, "fg") == 0)
    {
      handleFg(1);
    }
    else
    {
      char *args[] = {"./hi", NULL};
      createProcess(args, 0, jobs);
    }

    printJobs(jobs);
  }
}