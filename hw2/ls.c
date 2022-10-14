#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

// dont print any file that starts with .

int cmpcasestr(const void *stringOne, const void *stringTwo)
{
    return strcasecmp(*(const char **)stringOne, *(const char **)stringTwo);
}

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        printf("invalid arguments\n");
        return EXIT_FAILURE;
    }
    int flag;
    if (argc == 1)
        flag = 0;
    else if (argc == 2 && !strcmp(argv[1], "-l"))
        flag = 1;
    else
    {
        printf("invalid arguments\n");
        return EXIT_FAILURE;
    }
    struct dirent *dir;
    DIR *dirp = opendir(".");
    errno = 0;
    char **filenames = malloc(sizeof(char *));
    int size = 1;
    int idx = 0;
    while ((dir = readdir(dirp)) != NULL)
    {
        if (dir->d_name[0] != '.')
        {
            int filenameLen = strlen(dir->d_name);
            char *filename = malloc((filenameLen + 1) * sizeof(char));
            strcpy(filename, dir->d_name);
            filenames[idx] = filename;
            idx++;
            if (idx == size)
            {
                char **temp = malloc(2 * size * sizeof(char *));
                for (int i = 0; i < idx; i++)
                    temp[i] = filenames[i];
                free(filenames);
                filenames = temp;
                size *= 2;
            }
        }
    }

    closedir(dirp);
    if (errno)
    {
        printf("error!\n");
        return EXIT_FAILURE;
    }

    qsort(filenames, idx, sizeof(char *), cmpcasestr);
    for (int i = 0; i < idx; i++)
    {
        if (flag)
        {
            struct stat fileStat;
            int statVal = stat(filenames[i], &fileStat);
            if (statVal)
            {
                printf("error!\n");
                return EXIT_FAILURE;
            }

            // https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
            // type
            printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");

            // permissions
            printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
            printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
            printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
            printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
            printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
            printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
            printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
            printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
            printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");

            // username
            struct passwd *pwd = getpwuid(fileStat.st_uid);
            if (pwd == NULL)
                printf(" %d", fileStat.st_uid);
            else if (errno)
            {
                printf("error!\n");
                return EXIT_FAILURE;
            }
            else
                printf(" %s", pwd->pw_name);

            // groupname
            struct group *grp = getgrgid(fileStat.st_gid);
            if (grp == NULL)
                printf(" %d", fileStat.st_gid);
            else if (errno)
            {
                printf("error!\n");
                return EXIT_FAILURE;
            }
            else
                printf(" %s", grp->gr_name);

            // size
            printf(" %ld", fileStat.st_size);

            // time modified
            struct tm *ts;
            ts = localtime(&fileStat.st_mtime);
            if (ts == NULL || errno)
            {
                printf("error!\n");
                return EXIT_FAILURE;
            }
            const char *months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
            printf(" %s", months[ts->tm_mon]);
            printf(" %d", ts->tm_mday);
            printf(" %d:", ts->tm_hour);
            if (ts->tm_min / 10 == 0)
                printf("0");
            printf("%d", ts->tm_min);

            // filename
            printf(" %s\n", filenames[i]);
        }
        else
            printf("%s\n", filenames[i]);
    }

    for (int i = 0; i < idx; i++)
        free(filenames[i]);
    free(filenames);
    return EXIT_SUCCESS;
}
