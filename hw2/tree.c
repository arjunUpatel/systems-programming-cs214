#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

void printDir(char* dirname, int depth) {
  struct dirent* dir;
  DIR* dirp = opendir(dirname);

  if (dirp != NULL) {
    while ((dir = readdir(dirp)) != NULL) {
      if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
        for (int i = 0; i < depth; i++) {
          printf("  ");
        }
        printf("- %s\n", dir->d_name);
        printDir(dir->d_name, depth+1);
      }
    }
  }

  closedir(dirp);
}

int main() {
  printf(".\n");
  printDir(".", 0);
}