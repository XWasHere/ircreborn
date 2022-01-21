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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

#include <common/logger.h>
#include <common/attrib.h>

logger_t::logger_t() {
    this->channel_count = 0;
    this->width         = 0;
    this->channels      = (logger_channel_t**)malloc(sizeof(void*) * 16);
}

logger_t::~logger_t() {
    for (int i = 0; i < this->channel_count; i++) {
        if (this->channels[i]->kill_on_clean) free(this->channels[i]);
    }
    free(this->channels);
}

int logger_t::add_channel(int fd, char* name) {
    this->channel_count++;
    logger_channel_t** r = (logger_channel_t**)realloc(this->channels, sizeof(void*) * this->channel_count);
    if (r) this->channels = r;

    logger_channel_t* channel = (logger_channel_t*)malloc(sizeof(logger_channel_t));

    channel->kill_on_clean = 1;
    channel->fd = fd;
    channel->name = (char*)malloc(strlen(name) + 1);
    
    strcpy(channel->name, name);

    int width = strlen(name);
    this->width = width > this->width ? width : this->width;

    this->channels[this->channel_count - 1] = channel;
    
    return this->channel_count - 1;
}

void logger_t::log(int id, char* format, ...) {
    va_list args;
    va_start(args, format);
    
    logger_channel_t* channel = this->channels[id];

    char* data;
    char* data2;
    
    va_list fargs;
    va_copy(fargs, args);

    int flen = strlen(format);
    int dlen = 0;
    for (int i = 0; i < flen; i++) {
        if (format[i] == '%') {
            i++;
            if (format[i] == '%') {
                dlen++;
            } else if (format[i] == 's') {
                dlen += strlen(va_arg(fargs, char*));
            } else if (format[i] == 'i') {
                dlen += 32;
                va_arg(fargs, int);
            } else if (format[i] == 'x') {
                dlen += 16;
                va_arg(fargs, int);
            } else {
                dlen++;
            }
        } else {
            dlen++;
        }
    }
    
    data2 = (char*)malloc(dlen + 1);
    vsprintf(data2, format, args);
    
    data = (char*)malloc(dlen + 5 + this->width);
    
    sprintf(data, " %-*s | %s", this->width, channel->name, data2);
    
    write(channel->fd, data, strlen(data));

    free(data);
    free(data2);
}