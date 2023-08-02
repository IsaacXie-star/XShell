#include "lib.h"
#include "command.h"
#include <fcntl.h>

#define MAXARGS 10
#define NULL ((void *)0)

#define clearScreen "\x1b\x5b\x48\x1b\x5b\x32\x4a"

void _start() {
  static char buf[100];

  // Read and run input commands.
  while (getcmd(buf, sizeof(buf)) >= 0) {
    if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ') {
      // Chdir must be called by the parent, not the child.
      buf[strlen(buf) - 1] = 0;  // chop \n
      if (syscall(SYS_chdir, buf + 3) < 0)
        print("cannot cd ", buf + 3, "\n", NULL);
      continue;
    }
    if (buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't' && (buf[4] == ' ' || buf[4] == '\n')) {
      // exit the shell program
      break; // break the loop and exit
    }
    if (buf[0] == 'c' && buf[1] == 'l' && buf[2] == 'e' && buf[3] == 'a' && buf[4] == 'r' && (buf[5] == ' ' || buf[5] == '\n')) {
      // do a clear
      print(clearScreen, NULL);
      continue;
    }
    if (syscall(SYS_fork) == 0) runcmd(parsecmd(buf));
    syscall(SYS_wait4, -1, 0, 0, 0);
  }
  syscall(SYS_exit, 0);
}
