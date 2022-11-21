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

// BUG: Bg jobs that end do not change status
// BUG: Ctrl-c exits shell
// TODO: Change kill to killpg

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
    process->inputParse->ampersandPresent = true;
  }
}

void updateJobStatus(Process *process)
{
  int wstatus;
  pid_t pid = 0;
  bool flag = false;
  process->inputParse->ampersandPresent = false;

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
      printf("\n");
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
  if (process->inputParse->ampersandPresent)
    printf("&");
  printf("\n");
}

void killJob(Stack *jobStack, Process *process)
{
  if (process != NULL)
  {
    kill(process->pid, SIGTERM);
    waitpid(process->pid, NULL, WNOHANG);
    freeProcess(removeElem(jobStack, process->jid));
  }
}

void exitShell(InputParse *inputParse, Stack *jobStack)
{
  Process *process = pop(jobStack);
  while (process != NULL)
  {
    if (process->status == 0 || process->status == 1)
    {
      killpg(process->pid, SIGHUP);
    }
    if (process->status == 1)
    {
      killpg(process->pid, SIGCONT);
    }
    process = pop(jobStack);
  }
  freeInputParse(inputParse);
  freeStack(jobStack);
  exit(0);
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

bool runBuiltIn(InputParse *inputParse, Stack *jobStack, pid_t shell_pid)
{
  if (strcmp(inputParse->parsedInput[0], "bg") == 0)
  {
    for (int i = 1; inputParse->parsedInput[i] != NULL; i++)
    {
      if (inputParse->parsedInput[i][0] == '%')
      {
        char *jidStr = inputParse->parsedInput[i] + 1;
        int jid = strtol(jidStr, NULL, 10);
        Process *process = getElem(jobStack, jid);
        if (process == NULL)
        {
          printf("bg: (%d) - No such job\n", jid);
        }
        else
        {
          putProcessInBackground(process);
        }
      }
      else
      {
        printf("bg: (%s) - Operation not permitted\n", inputParse->parsedInput[i]);
      }
    }
  }
  else if (strcmp(inputParse->parsedInput[0], "cd") == 0)
  {
  }
  else if (strcmp(inputParse->parsedInput[0], "exit") == 0)
  {
    exitShell(inputParse, jobStack);
  }
  else if (strcmp(inputParse->parsedInput[0], "fg") == 0)
  {
    for (int i = 1; inputParse->parsedInput[i] != NULL; i++)
    {
      if (inputParse->parsedInput[i][0] == '%')
      {
        char *jidStr = inputParse->parsedInput[i] + 1;
        int jid = strtol(jidStr, NULL, 10);
        Process *process = getElem(jobStack, jid);
        if (process == NULL)
        {
          printf("fg: (%d) - No such job\n", jid);
        }
        else
        {
          putProcessInBackground(process);
          putProcessInForeground(jobStack, process, shell_pid);
        }
      }
      else
      {
        printf("fg: (%s) - Operation not permitted\n", inputParse->parsedInput[i]);
      }
    }
  }
  else if (strcmp(inputParse->parsedInput[0], "jobs") == 0)
  {
    updateJobs(jobStack);
    printStack(jobStack);
  }
  else if (strcmp(inputParse->parsedInput[0], "kill") == 0)
  {
    for (int i = 1; inputParse->parsedInput[i] != NULL; i++)
    {
      if (inputParse->parsedInput[i][0] == '%')
      {
        char *jidStr = inputParse->parsedInput[i] + 1;
        int jid = strtol(jidStr, NULL, 10);
        Process *process = getElem(jobStack, jid);
        if (process == NULL)
        {
          printf("kill: (%d) - No such job\n", jid);
        }
        else
        {
          killJob(jobStack, process);
        }
      }
      else
      {
        printf("kill: (%s) - Operation not permitted\n", inputParse->parsedInput[i]);
      }
    }
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
    if (runBuiltIn(inputParse, jobStack, shell_pid) == true)
    {
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
    updateJobs(jobStack);
    Process *process = addJob(jobStack, pid, inputParse);
    sigset_t s;
    sigemptyset(&s);
    sigaddset(&s, SIGTTOU);
    // sigaddset(&s, SIGINT);
    // sigaddset(&s, SIGCONT);
    // sigaddset(&s, SIGTSTP);
    sigprocmask(SIG_SETMASK, &s, NULL);
    if (inputParse->ampersandPresent)
    {
      // Unblock SIGTERM for kill to work
      sigset_t s;
      sigemptyset(&s);
      sigaddset(&s, SIGTERM);
      sigprocmask(SIG_UNBLOCK, &s, NULL);
      printf("[%d] %d\n", process->jid, pid);
      putProcessInBackground(process);
    }
    else
    {
      putProcessInForeground(jobStack, process, shell_pid);
    }
  }
  else
    perror("fork");
}
