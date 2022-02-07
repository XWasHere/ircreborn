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

#ifdef LAST_RESORT
#include <setjmp.h>
#include <signal.h>

// this code will decrease your lifespan by 10 years
static jmp_buf agony;

// capture and ignore SIGSEGV
static void trolled(int hi) {
    longjmp(agony, 1);
    return;
}

int check_pointer_valid(void* addr) {
    int valid = 1;
    // setup the handler
    signal(SIGSEGV, trolled);
    if (setjmp(agony)) {
        // handler will jump to here    
        valid = 0;
        // jump past the dereference
        goto recover;
    }
    // dereference the pointer. if its invalid well get a segfault
    volatile char a = *(char*)addr;
    recover:
    sigset_t stuff;
    // fix this
    signal(SIGSEGV, SIG_DFL);
    // clear stuff
    sigemptyset(&stuff);
    // apply to SIGSEGV
    sigaddset(&stuff, SIGSEGV);
    // reset the action
    sigprocmask(SIG_UNBLOCK, &stuff, 0);
    return valid;
}
#endif