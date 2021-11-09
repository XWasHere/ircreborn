#ifndef IRCREBORN_COMMON_UTIL_H
#define IRCREBORN_COMMON_UTIL_H

#ifdef  _WIN32
#define WOE32
#endif 

#include <string.h>

#define STREQ(a, b) (strcmp(a, b) == 0)

#define FMT_INFO(a) " == | " a
#define FMT_WARN(a) " !  | " a
#define FMT_FATL(a) " !! | " a // i know its spelled fatal.
#define FMT_CONT(a) "    | " a

char* format_last_error();
char* format_error(int errorcode);
void  gdb_breakpoint();

#endif