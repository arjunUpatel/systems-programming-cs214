#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

void findFile(char *dirname, char *path, char *pattern)
{
    struct dirent *dir;
    DIR *dirp = opendir(path);

    if (strlen(dirname) > 0 && strstr(dirname, pattern) != NULL)
    {
        printf("%s\n", path);
    }
    if (dirp != NULL)
    {
        while ((dir = readdir(dirp)) != NULL)
        {
            if (dir->d_name[0] != '.' && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
            {
                char *newPath = malloc(strlen(path) + strlen(dir->d_name) + 2);
                strcpy(newPath, path);
                strcat(newPath, "/");
                strcat(newPath, dir->d_name);
                findFile(dir->d_name, newPath, pattern);
                free(newPath);
            }
        }
    }
    closedir(dirp);
}

int main(int argc, char **argv)
{
    if (argc > 2 || argc < 2)
    {
        printf("invalid arguments\n");
        return EXIT_FAILURE;
    }
    char *pattern = argv[1];
    findFile("", ".", pattern);
}
