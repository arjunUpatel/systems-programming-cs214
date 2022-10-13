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

int cmpcasestr(const void *stringOne, const void *stringTwo)
{
    return strcasecmp(*(const char **)stringOne, *(const char **)stringTwo);
}

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        printf("Invalid arguments\n");
        return EXIT_FAILURE;
    }
    int flag;
    if (argc == 1)
        flag = 0;
    else if (argc == 2 && !strcmp(argv[1], "-l"))
        flag = 1;
    else
    {
        printf("Invalid arguments\n");
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
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
        {
            int filenameLen = strlen(dir->d_name);
            char *filename = malloc((filenameLen + 1) * sizeof(char));
            strcpy(filename, dir->d_name);
            strcat(filename, "\0");
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
        printf("error!\n");

    qsort(filenames, idx, sizeof(char *), cmpcasestr);
    for (int i = 0; i < idx; i++)
    {
        if (flag)
        {
            char *pathname = malloc((strlen(filenames[i]) + 3) * sizeof(char));
            strcpy(pathname, "./");
            strcat(pathname, filenames[i]);
            strcat(pathname, "\0");
            free(pathname);
            struct stat fileStat;
            stat(filenames[i], &fileStat);

            // type
            printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");

            // permissions
            // https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
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
            struct passwd *uname = getpwuid(fileStat.st_uid);
            printf(" %s", uname->pw_name);
            // remember to print id if the name is not found

            // groupname
            struct group *grp = getgrgid(fileStat.st_gid);
            printf(" %s", grp->gr_name);

            // size
            printf(" %ld", fileStat.st_size);

            // time modified
            struct tm ts;
            ts = *localtime(&fileStat.st_mtime);
            const char *months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
            printf(" %s", months[ts.tm_mon - 1]);
            printf(" %d", ts.tm_mday);
            printf(" %d:", ts.tm_hour);
            if (ts.tm_min / 10 == 0)
                printf("0");
            printf("%d", ts.tm_min);

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
