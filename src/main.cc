/*
    ircreborn (the bad discord alternative)
    Copyright (C) 2021 IRCReborn Devs

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <common/args.h>
#include <common/util.h>
#include <client/client.h>
#include <server/server.h>
#include <tests/tests.h>
#include <common/logger.h>

int CHANNEL_INFO;
int CHANNEL_WARN;
int CHANNEL_FATL;
int CHANNEL_DBUG;

logger_t logger;

int main(int argc, char** argv) {
    logger = logger_t();

    CHANNEL_INFO = logger.add_channel(1, "INFO");
    CHANNEL_WARN = logger.add_channel(1, "WARN");
    CHANNEL_FATL = logger.add_channel(1, "FATL");
    CHANNEL_DBUG = logger.add_channel(1, "DBUG");
    
    parse_args(argc, argv);

    logger.log(CHANNEL_INFO, "%s v1.0.0\n", args_exec_name);
    
    if (args_is_server) {
        logger.log(CHANNEL_INFO, "starting server\n");
        server_main();
    } else if (args_test) {
        logger.log(CHANNEL_INFO, "running ircreborn tests\n");
        run_tests();
    } else {
        logger.log(CHANNEL_INFO, "starting client\n");
        client_main();
    }
}