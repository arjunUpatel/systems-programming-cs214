#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

int main() {
  struct dirent* dir;
  DIR* dirp = opendir(".");

  while ((dir = readdir(dirp)) != NULL) {

  }

  closedir(dirp);
}