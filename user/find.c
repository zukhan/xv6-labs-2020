#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/*
 * Write a simple version of the UNIX find program: find all the files in a
 * directory tree with a specific name. Your solution should be in the file
 * user/find.c.
 */
void find(char*,char*);
void print_if_match(char*,char*,char*);
char* get_full_path(char*,char*);
char* fmtname(char*);

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(2, "usage: find [path] [expression]\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}

void
find(char* path, char* pattern)
{
  int fd = open(path, 0);
  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type) {
    case T_FILE:
      print_if_match(path, fmtname(path), pattern);
      break;
    case T_DIR:
      struct dirent de;

      while(read(fd, &de, sizeof(de)) == sizeof(de)) {
        char* filename = de.name;
        if (de.inum == 0 ||
            strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
          continue;
        }
        char* full_path = get_full_path(path, filename);
        if(stat(full_path, &st) < -1){
          printf("find: cannot stat %s\n", full_path);
          free(full_path);
          continue;
        }
        switch(st.type) {
          case T_FILE:
            print_if_match(full_path, filename, pattern);
            break;
          case T_DIR:
            find(full_path, pattern);
            break;
        }
        free(full_path);
      }
      break;
  }
}

void
print_if_match(char* rel_path, char* filename, char* pattern)
{
  if (strcmp(filename, pattern) == 0) {
    printf("%s\n", rel_path);
  }
}

char*
get_full_path(char* prefix, char* filename)
{
  int prefix_len = strlen(prefix);
  int filename_len = strlen(filename);
  char* full_path = malloc(prefix_len + filename_len + 2);
  strcpy(full_path, prefix);
  char* p = full_path + strlen(full_path);
  *p++ = '/';
  memmove(p, filename, filename_len);
  p[filename_len] = 0;
  return full_path;
}

char*
fmtname(char* path)
{
  char *p;
  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--) { }
  p++;
  return p;
}
