#ifndef COMMAND_H
#define COMMAND_H

#define MAXARGS 10

// Parsed command representation
enum { EXEC = 1, REDIR, PIPE, LIST, BACK };

struct cmd {
  int type;
};

struct execcmd {
  int type;
  char *argv[MAXARGS], *eargv[MAXARGS];
};

struct redircmd {
  int type, fd, mode;
  char *file, *efile;
  struct cmd* cmd;
};

struct pipecmd {
  int type;
  struct cmd *left, *right;
};

struct listcmd {
  int type;
  struct cmd *left, *right;
};

struct backcmd {
  int type;
  struct cmd* cmd;
};

struct cmd* parsecmd(char*);

void runcmd(struct cmd* cmd);

int getcmd(char* buf, int nbuf);

// Command constructors
struct cmd* execcmd(void);

struct cmd* redircmd(struct cmd* subcmd, char* file, char* efile, int mode, int fd);

struct cmd* pipecmd(struct cmd* left, struct cmd* right);

struct cmd* listcmd(struct cmd* left, struct cmd* right);

struct cmd* backcmd(struct cmd* subcmd);

#endif
