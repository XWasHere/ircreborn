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

#ifndef IRCREBORN_TEST_UTIL_H
#define IRCREBORN_TEST_UTIL_H

#include <string.h>

#include <common/util.h>

#define TEST_STOP() { \
    goto __T_STOP__; \
};

#define TEST_PASS(msg, ...) { \
    printf(FMT_INFO("test \"%s\" completed normally: " #msg "\n"), __T_NAME__, ## __VA_ARGS__); \
    TEST_STOP(); \
};

#define TEST_FAIL(msg, ...) { \
    printf(FMT_FATL("test \"%s\" failed: " #msg "\n"), __T_NAME__, ## __VA_ARGS__); \
    TEST_STOP(); \
};

#define INT_ASSERT_EQUALS(a, b) {\
    if (a != b) { \
        TEST_FAIL("assertion failed %i != %i", a, b); \
    } \
};

#define STRING_ASSERT_EQUALS(a, b) { \
    if (strcmp(a, b) != 0) { \
        TEST_FAIL("assertion failed \"%s\" != \"%s\"", a, b); \
    } \
};

#define TEST(name, ...) { \
    __label__ __T_STOP__; \
    const char* __T_NAME__ = #name; \
    printf(FMT_INFO("running test \"" #name "\"\n")); \
    __VA_ARGS__ \
    TEST_PASS("no description provided"); \
    __T_STOP__:; \
};

#endif
