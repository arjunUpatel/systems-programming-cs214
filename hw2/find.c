#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

void findFile(char *dirname, char *path, char *pattern)
{
    struct dirent *dir;
    DIR *dirp = opendir(path);

    if (strstr(dirname, pattern) != NULL)
    {
        printf("%s\n", path);
    }
    if (dirp != NULL)
    {
        while ((dir = readdir(dirp)) != NULL)
        {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
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
    char *pattern = argv[1];
    findFile(".", ".", pattern);
}
