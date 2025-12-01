#include "kernel/fs.h"
#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc < 1) {
    fprintf(2, "Usage: xargs <commmands>\n");
    exit(1);
  }

  if (argc + 1 > MAXARG) {
    fprintf(2, "xargs: too many args\n");
    exit(1);
  }

  // 若 argc == 1，则 param[0] = "echo"
  // 若 argc > 1，则从 argv[1 ++] 开始全部拷贝到 param[0 ++];
  char *params[MAXARG];
  if (argc == 1) {
    params[0] = "echo";
  } else {
    for (int i = 1; i < argc; i++) {
      params[i - 1] = argv[i];
    }
  }

  char buf[512];

  // 记录读取的字节数
  int n = 0;
  int idx = 0;


  while (1) {
    while ((n = read(0, &buf[idx], 1) > 0)) {

      if (n == 0 || buf[idx] == '\n') {
        // 去掉换行符
        buf[idx] = '\0';
        idx = 0;
        break;

      } else {
        idx++;
      }
    }

    if (n == 0)
      break;

    if (n < 0) {
      fprintf(2, "xargs: read error\n");
      exit(1);
    }

    if (argc == 1) {
      params[argc] = buf;
    }
    else {
      params[argc - 1] = buf;
    }

    if (fork() == 0) {
      // child process
      exec(params[0], params);
      exit(1);
    }
    else {
      // parent process
      wait(0);
    }
    
  }  



  exit(0);
}
