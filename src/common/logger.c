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

// Finally! A logging library that doesnt execute random code

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

#include <common/logger.h>
#include <common/attrib.h>

int CHANNEL_INFO;
int CHANNEL_WARN;
int CHANNEL_FATL;
int CHANNEL_DBUG;

static logger_channel_t** channels;
static int                channel_count;
static int                channel_width;

// this should be called automatically, but if it isnt then
// this should be called before any other logger functions
// not doing so will result in undefined behavior
__attribute__((constructor))
void logger_init() {
    // fill some values
    channels      = malloc(1);
    channel_count = 0;
    channel_width = 0;

    // add some default channels
    CHANNEL_INFO = logger_add_channel(0, "==");
    CHANNEL_WARN = logger_add_channel(0, "! ");
#ifdef LOGGER_USE_STDERR_FOR_ERRORS
    CHANNEL_FATL = logger_add_channel(2, "!!");
#else
    CHANNEL_FATL = logger_add_channel(0, "!!");
#endif
    CHANNEL_DBUG = logger_add_channel(0, "**");
}

// add a logger channel. usually the four default ones will be enough but idk
int logger_add_channel(int fd, char* name) {
    channel_count++;
    channels = realloc(channels, sizeof(void*) * channel_count);

    logger_channel_t* channel = malloc(sizeof(logger_channel_t));

    channel->fd   = fd;
    channel->name = name;
    
    channels[channel_count - 1] = channel;

    int width = strlen(name);
    
    channel_width = width > channel_width ? width : channel_width;

    return channel_count - 1;
}

void logger_log(int channel, char* format, ...) {
    va_list hi;
    va_start(hi, format);

    char* data  = malloc(64);
    char* data2 = malloc(64);

    sprintf(data, " % -*s | %s", channel_width, channels[channel]->name, format);
    vsprintf(data2, data, hi);

    write(channels[channel]->fd, data2, strlen(data2));

    free(data);
    free(data2);
}

// this is called when the program terminates
__attribute__((destructor))
void logger_fini() {
    for (int i = 0; i < channel_count; i++) free(channels[i]);
    free(channels);
}