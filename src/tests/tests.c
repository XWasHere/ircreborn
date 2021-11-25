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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <common/util.h>
#include <networking/types.h>

#include <tests/util.h>
#include <tests/tests.h>

int run_tests() {
    TEST(nettype_int, {
        int in    = 42069;
        char* out = malloc(255);

        write_int(out, in);
        int res = read_int(out);

        INT_ASSERT_EQUALS(res, in);

        free(out);
    });

    TEST(nettype_string, {
        char* in  = "if youre seeing this, the thing worked";
        char* out = malloc(255);

        write_string(out, in, strlen(in));
        nstring_t* res = read_string(out);

        INT_ASSERT_EQUALS(res->len, (int)strlen(in));
        STRING_ASSERT_EQUALS(res->str, in);

        free(out);
        free(res);
    });

    TEST(nettype_true, {
        int   in  = 1;
        char* out = malloc(255);

        write_bool(out, in);
        int res = read_bool(out);

        INT_ASSERT_EQUALS(res, 1);

        free(out);
    });

    TEST(nettype_false, {
        int   in  = 0;
        char* out = malloc(255);

        write_bool(out, in);
        int res = read_bool(out);

        INT_ASSERT_EQUALS(res, 0);

        free(out);
    });

    return 0;
}
