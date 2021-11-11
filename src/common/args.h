#ifndef IRCREBORN_COMMON_ARGS_H
#define IRCREBORN_COMMON_ARGS_H

extern char* args_exec_name;
extern int   args_is_server;
extern char* args_config_path;
extern int   args_test;

void parse_args(int argc, char** argv);

#endif
