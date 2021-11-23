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
#include <stdint.h>

#ifdef WIN32
int pread(int fd, void* buf, int count, int offset) {  
    memset(buf, 0, count); 
    int ooffset = lseek(fd, 0l, SEEK_CUR);
    lseek(fd, offset, SEEK_SET);
    int len = read(fd, buf, count);
    lseek(fd, ooffset, SEEK_SET);
    return len;
}
#endif