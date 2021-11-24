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


int main(int argc, char** argv) {
    parse_args(argc, argv);

    PINFO("%s v1.0.0\n", args_exec_name);
    
    if (args_is_server) {
        PINFO("starting server\n");
        server_main();
    } else if (args_test) {
        PINFO("running ircreborn tests\n");
        run_tests();
    } else {
        PINFO("starting client\n");
        client_main();
    }
}
