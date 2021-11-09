#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <common/util.h>
#define INIT_STRING(name) name = malloc(1); name[0] = 0;

char* args_exec_name;
int   args_is_server;
char* args_config_path;

void parse_args(int argc, char** argv) {
    args_exec_name = argv[0];

#ifdef WIN32
    args_config_path = getenv("USERPROFILE");
#else
    args_config_path = getenv("HOME");
#endif
    strcat(args_config_path, "/.ircreborn");
    
#define next goto common_args_next;

    for (int i = 1; i < argc; i++) {
        if (STREQ(argv[i], "--server")) {
            args_is_server = 1;
            next;
        } else if (STREQ(argv[i], "--config")) {
            args_config_path = argv[i + 1];
            i += 1;
            next;
        }

        common_args_next:;
    }

#undef next
}