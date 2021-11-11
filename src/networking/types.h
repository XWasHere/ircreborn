#ifndef NETWORKING_TYPES_H
#define NETWORKING_TYPES_H

typedef struct network_string nstring_t;
struct network_string {
    int   len;
    char* str;
};

void       write_string(void* buf, char* str, int len);
nstring_t* read_string(void* buf);
void       write_int(void* buf, int i);
int        read_int(void* buf);
void       write_bool(void* buf, int v);
int        read_bool(void* buf);

#endif
