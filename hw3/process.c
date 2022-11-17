#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "stack.h"
#include "process.h"
#include "parser.h"

extern pid_t foregroundPID;

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

void printJob(Process *process)
{
  printf("[%d] %d %s ", process->jid, process->pid, statusToString(process->status));
  for (int i = 0; process->inputParse->parsedInput[i] != NULL; i++)
    printf("%s ", process->inputParse->parsedInput[i]);
}

void printJobs(Process **jobs, int numJobs)
{
  for (int i = 0; i < numJobs; i++)
    printJob(jobs[i]);
}

bool isDirectory(const char *path)
{
  struct stat stats;
  stat(path, &stats);

  if (S_ISDIR(stats.st_mode))
    return true;
  return false;
}

bool isCommand(char *arg)
{
  if (arg[0] == '.' || arg[0] == '/')
    return false;
  return true;
}

bool isValidPath(char *arg)
{
  if (access(arg, F_OK) == -1)
    return false;
  return true;
}

int isBuiltIn(char *arg)
{
  char *builtIns[6] = {"bg", "cd", "exit", "fg", "jobs", "kill"};
  for (int i = 0; i < 6; i++)
  {
    if (strcmp(arg, builtIns[i]) == 0)
      return i;
  }
  return -1;
}

void freeProcess(Process *process)
{
  freeInputParse(process->inputParse);
  free(process);
}

void createProcess(InputParse *inputParse, Stack *jobStack, pid_t shell_pid)
{
  char *pathname;
  if (isCommand(inputParse->parsedInput[0]))
  {
    // check if it is built in
    int builtInFuncIdx = isBuiltIn(inputParse->parsedInput[0]);
    if (builtInFuncIdx != -1)
    {
      // do built in stuff
      // possibly make an array of function pointers and map to the correspoing built in function's function and then return
    }
    else
    {
      char *searchPaths[2] = {"/usr/bin/", "/bin/"};
      int i = 0;
      bool validPath = false;
      do
      {
        pathname = malloc((strlen(inputParse->parsedInput[0]) + strlen(searchPaths[i]) + 1) * sizeof(char));
        strcpy(pathname, searchPaths[i]);
        strcat(pathname, inputParse->parsedInput[0]);
        validPath = isValidPath(pathname);
        if (validPath)
          break;
        free(pathname);
        i++;
      } while (i < 2);
      // command was not found
      if (!validPath)
      {
        printf("%s: Command not found\n", inputParse->parsedInput[0]);
        return;
      }
    }
  }
  else
  {
    pathname = malloc((strlen(inputParse->parsedInput[0]) + 1) * sizeof(char));
    strcpy(pathname, inputParse->parsedInput[0]);
    if (isDirectory(pathname))
    {
      printf("%s: Is a directory\n", pathname);
      free(pathname);
      return;
    }
    if (!isValidPath(pathname))
    {
      printf("%s: No such file or directory\n", pathname);
      free(pathname);
      return;
    }
  }

  char **args = malloc(inputParse->parseLen * sizeof(char *));
  args[0] = pathname;
  for (int i = 1; i < inputParse->parseLen; i++)
    args[i] = inputParse->parsedInput[i];

  pid_t pid;
  int status;
  pid = fork();
  if (pid == 0)
  {
    // do child stuff
    setpgid(pid, pid);
    // printf("EXECV %s", args[0]);
    execv(args[0], args);
    exit(EXIT_SUCCESS);
  }
  if (pid > 0)
  {
    int jid = jobStack->head != NULL ? jobStack->head->element->jid + 1 : 1;
    Process *process = malloc(sizeof(Process));
    process->jid = jid;
    process->pid = pid;
    process->status = 0;
    process->inputParse = inputParse;
    push(jobStack, process);
    // do parent stuff
    if (inputParse->ampersandPresent)
      printf("[%d] %d\n", process->jid, pid);
    else
    {
      tcsetpgrp(STDIN_FILENO, pid);
      // foregroundPID = pid;
      do
      {
        pid = wait(&status);
        if (pid == -1)
        {
          perror("waitpid");
          exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status))
        {
          printf("exited, status=%d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
          printf("killed by signal %d\n", WTERMSIG(status));
        }
        else if (WIFSTOPPED(status))
        {
          printf("stopped by signal %d\n", WSTOPSIG(status));
        }
        else if (WIFCONTINUED(status))
        {
          printf("continued\n");
        }
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
      tcsetpgrp(STDIN_FILENO, shell_pid);
      Process *process = removeElem(jobStack, jid);
      freeProcess(process);
      // foregroundPID = -1;
      if (WIFSIGNALED(status))
      {
        printf("\n[%d] %d terminated by signal %d\n", 0, pid, WTERMSIG(status));
      }
    }
  }
  else
  {
    // error while creating child
    perror("fork");
  }
  free(pathname);
  free(args);
}
