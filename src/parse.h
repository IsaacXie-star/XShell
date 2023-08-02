#ifndef PARSE_H
#define PARSE_H

#include "command.h"

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

int gettoken(char** ps, char* es, char** q, char** eq);

int peek(char** ps, char* es, char* toks);

struct cmd* parsecmd(char* s);

struct cmd* parseline(char**, char*);

struct cmd* parsepipe(char**, char*);

struct cmd* parseexec(char**, char*);

struct cmd* parseblock(char** ps, char* es);

struct cmd* parseredirs(struct cmd* cmd, char** ps, char* es);

struct cmd* nulterminate(struct cmd*);

#endif