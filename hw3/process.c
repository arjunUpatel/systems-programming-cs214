#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "parser.h"
#include <sys/stat.h>

typedef struct Process
{
    int jid;
    pid_t pid;
    // 0 = running, 1 = stopped, 2 = terminated
    int status;
    char *command
} Process;

void createJob(Process **bgJobs, int numJobs, Process *process)
{
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

void printJob(Process *process)
{
    printf("[%d] %d %s %s\n", process->jid, process->pid, statusToString(process->status), process->command);
}

void printJobs(Process **bgJobs, int numJobs)
{
    for (int i = 0; i < numJobs; i++)
        printJob(bgJobs[i]);
}

// int isFileExistsStats(const char *path)
// {
//     struct stat stats;
//     stat(path, &stats);

//     // Check for file existence
//     if (stats.st_mode & F_OK)
//         return 1;
//     return 0;
// }

// int isDirectoryExists(const char *path)
// {
//     struct stat stats;
//     stat(path, &stats);

//     // Check for file existence
//     if (S_ISDIR(stats.st_mode))
//         return 1;
//     return 0;
// }

bool isCommand(char *arg)
{
    if (arg[0] == '.' || arg[0] == '/')
        return true;
    return false;
}

bool isValidPath(char *arg)
{
    struct stat stats;
    stat(arg, &stats);
    if ((stats.st_mode & F_OK) || S_ISDIR(stats.st_mode))
        return true;
    return false;
}

int isBuiltIn(char *arg)
{
    char **builtIns[6] = {{"bg"}, {"cd"}, {"exit"}, {"fg"}, {"jobs"}, {"kill"}};
    for (int i = 0; i < 6; i++)
    {
        if (strcmp(arg, builtIns[i]) == 0)
            return i;
    }
    return -1;
}

void createProcess(InputParse *inputParse, Process **bgJobs, int numJobs)
{
    char *pathname;
    if (isCommand(inputParse->parsedInput[0]))
    {
        // check if it is built in
        int *builtInFuncIdx = isBuiltIn(inputParse->parsedInput[0]);
        if (builtInFuncIdx != -1)
        {
            // do built in stuff
            // possibly make an array of function pointers and map to the correspoing built in function's function and then return
        }
        else
        {
            char **searchPaths[2] = {{"/usr/bin"}, {"/bin"}};
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
            } while (i < 2);
            // command was not found
            if (!validPath)
            {
                printf("%s: Command not found", inputParse->parsedInput[0]);
                return;
            }
        }
    }
    else
    {
        pathname = malloc((strlen(inputParse->parsedInput[0]) + 1) * sizeof(char));
        strcpy(pathname, inputParse->parsedInput[0]);
        if (!isValidPath(pathname))
        {
            printf("%s: No such file or directory", pathname);
            free(pathname);
            return;
        }
    }

    char **args = malloc(inputParse->parseLen * sizeof(char *));
    args[0] = pathname;
    for (int i = 1; i < inputParse->parseLen; i++)
        args[i] = inputParse->parsedInput[i];

    pid_t pid, w;
    int status;
    pid = fork();
    if (pid == 0)
    {
        // do child stuff
        execv(args[0], args);
        exit(EXIT_SUCCESS);
    }
    if (pid > 0)
    {
        // do parent stuff
        if (inputParse->ampersandPresent)
        {
            // add process to background
            Process *process = malloc(sizeof(Process));
            process->jid = bgJobs[numJobs]->jid + 1;
            process->pid = pid;
            process->status = 0;
            process->command = inputParse->parsedInput[0];
            createJob(bgJobs, numJobs, process);

            printf("[%d] %d\n", process->jid, pid);
        }
        else
        {
            do
            {
                w = wait(&status);
                if (w == -1)
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
        }
    }
    else
    {
        // error while creating child
        perror("fork");
        free(pathname);
        free(args);
    }
}
