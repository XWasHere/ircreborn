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

#ifndef IRCREBORN_LOGGER_H
#define IRCREBORN_LOGGER_H

// uncomment this if you like your errors on stderr
// #define LOGGER_USE_STDERR_FOR_ERRORS

extern int CHANNEL_INFO;
extern int CHANNEL_WARN;
extern int CHANNEL_FATL;
extern int CHANNEL_DBUG;

typedef struct logger_channel logger_channel_t ;
struct logger_channel {
    char* name;
    int   fd;
};

class logger_channel_t {
    public:
        // the name displayed to the right of the channel
        char* name;

        // the channel file descriptor
        int fd;
};

// logger
class logger_t {
    public:
        // the logger channels. 
        // you can refer to them by index to save a few bytes.
        logger_channel_t** channels;

};

int  logger_add_channel(int fd, char* name);

__attribute__((format(printf, 2, 3)))
void logger_log(int channel, char* data, ...);

#endif