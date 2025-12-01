#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"



void find(char* path, char* name)
{
  int fd;
  char buf[512], *p;
  struct stat st;
  struct dirent de;

  // 使用 open 打开起始位置
  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type != T_DIR) {
    fprintf(2, "find: <%s> is not a dir\n", path);
    close(fd);
    return; 
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;
    
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    
    // 若文件/目录的名字匹配，打印
    if (strcmp(de.name, name) == 0) {
      fprintf(1, "%s\n", buf);
    }

    if (stat(buf, &st) < 0) {
        printf("ls: cannot stat %s\n", buf);
        continue; 
    }

    if (st.type == T_DIR) {
      // 跳过 .. 和 .
      if (strcmp(de.name, "..") == 0 || strcmp(de.name, ".") == 0)
        continue;
      else
        find(buf, name);
    }
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "find <root> <name>\n");
    exit(1);
  }

  find(argv[1], argv[2]);


  
  exit(0);
}
