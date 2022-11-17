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

// extern pid_t foregroundPID;

// bool f_sigchld = false;

// void handle_sigchld(int signum)
// {
//   f_sigchld = true;
// }

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

// void printJobs(Stack **jobs, int numJobs)
// {
//   for (int i = 0; i < numJobs; i++)
//     printJob(jobs[i]);
// }

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

bool runBuiltIn(char *arg, Stack *jobStack)
{
  if (strcmp(arg, "bg") == 0)
  {
  }
  else if (strcmp(arg, "cd") == 0)
  {
  }
  else if (strcmp(arg, "exit") == 0)
  {
  }
  else if (strcmp(arg, "fg") == 0)
  {
  }
  else if (strcmp(arg, "jobs") == 0)
  {
    printStack(jobStack);
  }
  else if (strcmp(arg, "kill") == 0)
  {
  }
  else
  {
    return false;
  }
  return true;
}

void freeProcess(Process *process)
{
  freeInputParse(process->inputParse);
  free(process);
}

void createProcess(InputParse *inputParse, Stack *jobStack, pid_t shell_pid)
{
  // signal(SIGCHLD, handle_sigchld);
  char *pathname;
  if (isCommand(inputParse->parsedInput[0]))
  {
    // check if it is built in
    // returns true if it is built in command, false otherwise
    if (runBuiltIn(inputParse->parsedInput[0], jobStack) == true)
    {
      return;
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
  int status;
  sigset_t mask_all, mask_one, prev_one;
  sigfillset(&mask_all);
  sigemptyset(&mask_one);
  sigaddset(&mask_one, SIGCHLD);
  // sigset_t mask, prev;
  // // sigemptyset(&mask_sigchld);
  // sigfillset(&mask);
  // // sigaddset(&mask_sigchld, SIGCHLD);
  sigprocmask(SIG_BLOCK, &mask_one, &prev_one);
  pid_t pid = fork();
  if (pid == 0)
  {
    sigprocmask(SIG_SETMASK, &prev_one, NULL);
    setpgid(pid, pid);
    // printf("EXECV %s", args[0]);
    execv(args[0], args);
  }
  if (pid > 0)
  {
    sigprocmask(SIG_BLOCK, &mask_all, NULL);
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
      sigset_t s;
      sigemptyset(&s);
      sigaddset(&s, SIGCHLD);
      sigaddset(&s, SIGTTOU);
      sigprocmask(SIG_SETMASK, &s, NULL);
      int sig;
      int *sigptr = &sig;
      while (1)
      {
        int ret_val = sigwait(&mask_one, sigptr);
        printf("%d\n", ret_val);
        if (*sigptr == SIGCHLD)
        {
          tcsetpgrp(STDIN_FILENO, shell_pid);
          pid = waitpid(pid, &status, 0);
          Process *process = removeElem(jobStack, jid);
          freeProcess(process);
          break;
        }
      }

      // foregroundPID = pid;
      // do
      // {
      //   pid = wait(&status);
      //   if (pid == -1)
      //   {
      //     perror("waitpid");
      //     exit(EXIT_FAILURE);
      //   }

      //   if (WIFEXITED(status))
      //   {
      //     printf("exited, status=%d\n", WEXITSTATUS(status));
      //   }
      //   else if (WIFSIGNALED(status))
      //   {
      //     printf("killed by signal %d\n", WTERMSIG(status));
      //   }
      //   else if (WIFSTOPPED(status))
      //   {
      //     printf("stopped by signal %d\n", WSTOPSIG(status));
      //   }
      //   else if (WIFCONTINUED(status))
      //   {
      //     printf("continued\n");
      //   }
      // } while (!WIFEXITED(status) && !WIFSIGNALED(status));
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
