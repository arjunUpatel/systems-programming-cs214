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

void putProcessInBackground(Process *process)
{
  if (process->status == 1)
  {
    if (killpg(process->pid, SIGCONT) < 0)
    {
      perror("killpg");
      return;
    }
    process->status = 0;
  }
}

void updateJobStatus(Process *process)
{
  int wstatus;
  pid_t pid = 0;
  bool flag = false;
  do
  {
    pid = waitpid(process->pid, &wstatus, WUNTRACED | WNOHANG);
    if (pid <= 0)
      break;
    else
    {
      if (WIFEXITED(wstatus))
      {
        process->status = 3;
      }
      if (WIFSTOPPED(wstatus))
      {
        process->status = 1;
      }
      else if (WIFSIGNALED(wstatus))
      {
        int signum = WTERMSIG(wstatus);
        if (signum == 2)
          printf("[%d] bg signaled: [%d]\n", pid, signum);
        process->status = 2;
      }
      else if (WIFCONTINUED(wstatus))
      {
        process->status = 0;
      }
      flag = true;
    }
  } while (!flag);
}

// bug here
void updateJobs(Stack *jobStack)
{
  ListNode *prevNode = NULL;
  ListNode *node = jobStack->head;
  while (node != NULL)
  {
    updateJobStatus(node->element);
    if (node->element->status == 2 || node->element->status == 3)
    {
      if (prevNode == NULL)
      {
        jobStack->head = node->next;
        freeProcess(node->element);
        free(node);
        node = jobStack->head;
      }
      else
      {
        prevNode->next = node->next;
        freeProcess(node->element);
        free(node);
        node = prevNode->next;
      }
    }
    else
    {
      prevNode = node;
      node = node->next;
    }
  }
}

void putProcessInForeground(Stack *jobStack, Process *process, pid_t shell_pid)
{
  tcsetpgrp(STDIN_FILENO, process->pid);
  int wstatus;
  pid_t pid = 0;
  bool flag = false;
  do
  {
    pid = waitpid(process->pid, &wstatus, WUNTRACED);
    if (pid == -1)
    {
      perror("waitpid");
      exit(EXIT_FAILURE);
    }
    if (WIFEXITED(wstatus))
    {
      removeElem(jobStack, process->jid);
      freeProcess(process);
      flag = true;
    }
    else if (WIFSTOPPED(wstatus))
    {
      // killpg(process->pid, SIGTSTP);
      process->status = 1;
      flag = true;
    }
    else if (WIFSIGNALED(wstatus))
    {
      printf("\n[%d] %d terminated by signal %d\n", process->jid, process->pid, WTERMSIG(wstatus));
      removeElem(jobStack, process->jid);
      freeProcess(process);
      flag = true;
    }
    else if (WIFCONTINUED(wstatus))
    {
      printf("continued\n");
      flag = true;
    }
    // updateForegroundProcessStatus(jobStack, process, wstatus);
  } while (!flag);
  // while (!mark_process_status(pid, status) && !job_is_stopped(j) && !job_is_completed(j));
  tcsetpgrp(STDIN_FILENO, shell_pid);
}

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

Process *addJob(Stack *jobStack, pid_t pid, InputParse *inputParse)
{
  int jid = jobStack->head != NULL ? jobStack->head->element->jid + 1 : 1;
  Process *process = malloc(sizeof(Process));
  process->jid = jid;
  process->pid = pid;
  process->status = 0;
  process->inputParse = inputParse;
  push(jobStack, process);
  return process;
}

void printJob(Process *process)
{
  printf("[%d] %d %s ", process->jid, process->pid, statusToString(process->status));
  for (int i = 0; process->inputParse->parsedInput[i] != NULL; i++)
    printf("%s ", process->inputParse->parsedInput[i]);
  printf("\n");
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
  // signal(SIGCHLD, handle_sigchld);
  char *pathname;
  if (isCommand(inputParse->parsedInput[0]))
  {
    // check if it is built in
    int builtInFuncIdx = isBuiltIn(inputParse->parsedInput[0]);
    if (builtInFuncIdx != -1)
    {
      if (builtInFuncIdx == 4)
      {
        updateJobs(jobStack);
        printStack(jobStack);
      }
      freeInputParse(inputParse);
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
        freeInputParse(inputParse);
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
  // int status;
  sigset_t mask_all, mask_one, prev_one, prev_all;
  sigfillset(&mask_all);
  sigemptyset(&mask_one);
  sigaddset(&mask_one, SIGCHLD);
  sigprocmask(SIG_BLOCK, &mask_one, &prev_one);
  pid_t pid = fork();
  if (pid == 0)
  {
    sigprocmask(SIG_SETMASK, &prev_one, NULL);
    setpgid(pid, pid);
    execv(args[0], args);
  }
  if (pid > 0)
  {
    sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    free(pathname);
    free(args);
    Process *process = addJob(jobStack, pid, inputParse);
    if (inputParse->ampersandPresent)
    {
      printf("[%d] %d\n", process->jid, pid);
      putProcessInBackground(process);
    }
    else
    {
      sigset_t s;
      sigemptyset(&s);
      sigaddset(&s, SIGTTOU);
      // sigaddset(&s, SIGINT);
      // sigaddset(&s, SIGCONT);
      // sigaddset(&s, SIGTSTP);
      sigprocmask(SIG_SETMASK, &s, NULL);
      putProcessInForeground(jobStack, process, shell_pid);
    }
  }
  else
    perror("fork");
}
