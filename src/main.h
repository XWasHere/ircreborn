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

#ifndef IRCREBORN_MAIN_H
#define IRCREBORN_MAIN_H

#include <common/logger.h>

extern int CHANNEL_INFO;
extern int CHANNEL_WARN;
extern int CHANNEL_FATL;
extern int CHANNEL_VBS0;
extern int CHANNEL_DBUG;

extern logger_t* logger;

#endif