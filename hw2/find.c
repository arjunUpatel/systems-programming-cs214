#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

void findFile(DIR* dirp, char* path, char* pattern) {
  struct dirent* dir;

  while ((dir = readdir(dirp)) != NULL) {
    if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
      DIR* dirp2 = opendir(dir->d_name);
      if (dirp2 != NULL) {
        // dir is a directory, recursively search through it
        char* newPath = malloc(strlen(path) + strlen(dir->d_name) + 2);
        strcpy(newPath, path);
        strcat(newPath, "/");
        strcat(newPath, dir->d_name);

        findFile(dirp2, newPath, pattern);
        free(newPath);
      } else if (strstr(dir->d_name, pattern) != NULL) {
        // dir is a file that matches the pattern
        printf("%s/%s\n", path, dir->d_name);
      }
    }
  }

  closedir(dirp);
}

int main(int argc, char** argv) {
  char* pattern = argv[1];
  findFile(opendir("."), ".", pattern);
}
