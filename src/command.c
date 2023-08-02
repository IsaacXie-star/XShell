#include "command.h"
#include "lib.h"
#include <fcntl.h>

// Execute cmd.  Never returns.
void runcmd(struct cmd* cmd) {
  int p[2];
  struct backcmd* bcmd;
  struct execcmd* ecmd;
  struct listcmd* lcmd;
  struct pipecmd* pcmd;
  struct redircmd* rcmd;

  if (cmd == 0) syscall(SYS_exit, 1);

  switch (cmd->type) {
    case EXEC:
      ecmd = (struct execcmd*)cmd;
      if (ecmd->argv[0] == 0) syscall(SYS_exit, 1);
      syscall(SYS_execve, ecmd->argv[0], ecmd->argv, NULL);
      print("fail to exec ", ecmd->argv[0], "\n", NULL);
      break;

    case REDIR:
      rcmd = (struct redircmd*)cmd;
      syscall(SYS_close, rcmd->fd);
      if (syscall(SYS_open, rcmd->file, rcmd->mode, 0644) < 0) {
        print("fail to open ", rcmd->file, "\n", NULL);
        syscall(SYS_exit, 1);
      }
      runcmd(rcmd->cmd);
      break;

    case LIST:
      lcmd = (struct listcmd*)cmd;
      if (syscall(SYS_fork) == 0) runcmd(lcmd->left);
      syscall(SYS_wait4, -1, 0, 0, 0);
      runcmd(lcmd->right);
      break;

    case PIPE:
      pcmd = (struct pipecmd*)cmd;
      assert(syscall(SYS_pipe, p) >= 0);
      if (syscall(SYS_fork) == 0) {
        syscall(SYS_close, 1);
        syscall(SYS_dup, p[1]);
        syscall(SYS_close, p[0]);
        syscall(SYS_close, p[1]);
        runcmd(pcmd->left);
      }
      if (syscall(SYS_fork) == 0) {
        syscall(SYS_close, 0);
        syscall(SYS_dup, p[0]);
        syscall(SYS_close, p[0]);
        syscall(SYS_close, p[1]);
        runcmd(pcmd->right);
      }
      syscall(SYS_close, p[0]);
      syscall(SYS_close, p[1]);
      syscall(SYS_wait4, -1, 0, 0, 0);
      syscall(SYS_wait4, -1, 0, 0, 0);
      break;

    case BACK:
      bcmd = (struct backcmd*)cmd;
      if (syscall(SYS_fork) == 0) runcmd(bcmd->cmd);
      break;

    default:
      assert(0);
  }
  syscall(SYS_exit, 0);
}

int getcmd(char* buf, int nbuf) {
  print("(xsh) > ", NULL);
  for (int i = 0; i < nbuf; i++) buf[i] = '\0';

  while (nbuf-- > 1) {
    int nread = syscall(SYS_read, 0, buf, 1);
    if (nread <= 0) return -1;
    if (*(buf++) == '\n') break;
  }
  return 0;
}

// Command constructors
struct cmd* execcmd(void) {
  struct execcmd* cmd;

  cmd = zalloc(sizeof(*cmd));
  cmd->type = EXEC;
  return (struct cmd*)cmd;
}

struct cmd* redircmd(struct cmd* subcmd, char* file, char* efile, int mode,
                     int fd) {
  struct redircmd* cmd;

  cmd = zalloc(sizeof(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  return (struct cmd*)cmd;
}

struct cmd* pipecmd(struct cmd* left, struct cmd* right) {
  struct pipecmd* cmd;

  cmd = zalloc(sizeof(*cmd));
  cmd->type = PIPE;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd* listcmd(struct cmd* left, struct cmd* right) {
  struct listcmd* cmd;

  cmd = zalloc(sizeof(*cmd));
  cmd->type = LIST;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd* backcmd(struct cmd* subcmd) {
  struct backcmd* cmd;

  cmd = zalloc(sizeof(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  return (struct cmd*)cmd;
}