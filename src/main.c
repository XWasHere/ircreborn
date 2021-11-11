#include <stdio.h>
#include <common/args.h>
#include <common/util.h>
#include <client/client.h>
#include <server/server.h>

int main(int argc, char** argv) {
    parse_args(argc, argv);

    printf(FMT_INFO("%s v1.0.0\n"), args_exec_name);
    
    if (args_is_server) {
        printf(FMT_INFO("starting server\n"));
        server_main();
    } else {
        printf(FMT_INFO("starting client\n"));
        client_main();
    }
}
