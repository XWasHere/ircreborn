#ifndef IRCREBORN_COMPAT_H
#define IRCREBORN_COMPAT_H

#ifdef WIN32
int pread(int fd, void* buf, int count, int offset);
#endif

#endif