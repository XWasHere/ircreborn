#include <common/util.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WOE32 
#include <windows.h> // yayyyyyyyyyyyy
#endif

void gdb_breakpoint() {};

char* format_last_error() {
    int errorcode;
#ifdef WOE32
    errorcode = GetLastError();
#endif
    return format_error(errorcode);
}

char* format_error(int errorcode) {
    char* error;
    char* errordesc;

#ifdef WOE32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types" // nobody asked
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        0,
        errorcode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        &errordesc,
        0,
        0
    );
#pragma GCC diagnostic pop
#endif

    error = malloc(strlen(errordesc) + 14);
    memset(error, 0, strlen(errordesc) + 14);
    sprintf(error, "0x%08x - %s", errorcode, errordesc);

    return error;
}