#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include<ctype.h>

char* toLower(char* str) {
  for (int i = 0; i < strlen(str); i++) {
    str[i] = tolower(str[i]);
  }
  return str;
}

void printDir(char* path, int depth) {
  struct dirent* dir;
  DIR* dirp = opendir(path);

  if (dirp != NULL) {
    int maxSize = 5;
    int curSize = 0;
    char** orderedDirs = calloc(maxSize, sizeof(char*));

    while ((dir = readdir(dirp)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
        continue;
      }

      curSize++;
      if (curSize > maxSize) {
        maxSize *= 2;
        orderedDirs = realloc(orderedDirs, maxSize * sizeof(char*));
      }

      if (curSize == 1) {
        orderedDirs[curSize-1] = dir->d_name;
      } else {
        for (int i = 0; i < curSize-1; i++) {
          char orderedTemp[256];
          char dirTemp[256];
          strcpy(orderedTemp, orderedDirs[i]);
          strcpy(dirTemp, dir->d_name);
          if (strcmp(toLower(orderedTemp), toLower(dirTemp)) >= 0) {
            for (int j = curSize-2; j >= i; j--) {
              orderedDirs[j+1] = orderedDirs[j];
            }
            orderedDirs[i] = dir->d_name;
            break;
          }
          if (i == curSize-2) {
            orderedDirs[curSize-1] = dir->d_name;
          }
        }
      }
    }

    for (int i = 0; i < curSize; i++) {
      for (int i = 0; i < depth; i++) {
        printf("  ");
      }
      printf("- %s\n", orderedDirs[i]);

      char* newPath = malloc(strlen(path) + strlen(orderedDirs[i]) + 2);
      strcpy(newPath, path);
      strcat(newPath, "/");
      strcat(newPath, orderedDirs[i]);
      printDir(newPath, depth+1);
      free(newPath);
    }

    free(orderedDirs);
  }

  closedir(dirp);
}

int main() {
  printf(".\n");
  printDir(".", 0);
}