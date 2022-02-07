/*
    ircreborn (the bad discord alternative)
    Copyright (C) 2022 IRCReborn Devs

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

#include <common/util.h>

typedef struct logger_channel logger_channel_t;
struct logger_channel {
    // set by logger_t to identify that this channel was
    // created by add_channel and should be removed with the logger
    bool kill_on_clean;

    // the name displayed to the right of the channel
    char* name;

    // the channel file descriptor
    int fd;

    // self explanatory
    int enabled;
};

// logger
class logger_t {
    private:
        // how much space is on the left
        int width;

    public:
        // the logger channels. 
        // you can refer to them by index to save a few bytes.
        logger_channel_t** channels;

        // the number of channels the logger has (duh)
        int channel_count;

        // yeah
        logger_t();

        // bye
        ~logger_t();

        // add a channel and return its id
        int add_channel(int fd, char* name);

        // log
        __attribute__ (( format(printf, 3, 4) ))
        void log(int channel, char* data, ...);

        // log but with a channel
        __attribute__ (( format(printf, 3, 4) ))
        void log(logger_channel_t* channel, char* data, ...);

        // thing
        void vlog(logger_channel_t* channel, char* data, va_list args);
};

#endif