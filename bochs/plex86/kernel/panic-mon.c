/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  panic-mon.c:  Monitor panic facility.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#include "plex86.h"
#define IN_MONITOR_SPACE
#include "monitor.h"

extern int mon_vprint(vm_t *vm, char *fmt, va_list args);


/*======================================== */
/* Only to be used in monitor/guest space! */
/*======================================== */

  void
monpanic(vm_t *vm, char *fmt, ...)
{
    monprint(vm, "plex86 panic: ");
    if (fmt) 
    {
        va_list args;
        va_start(args, fmt);
        mon_vprint(vm, fmt, args);
        va_end(args);
    }

loop:

    CLI();

    vm->mon_request = MonReqPanic;
    vm->guest.__mon2host();

    /* mon2host() should never return in this case.  In case it ever */
    /* does because our logic is broken, keep returning back to */
    /* the host so we at least don't hang the machine. */
    goto loop;
}

  void
monpanic_nomess(vm_t *vm)
{
loop:
    CLI();
    vm->mon_request = MonReqPanic;
    vm->guest.__mon2host();

    /* mon2host() should never return in this case.  In case it ever */
    /* does because our logic is broken, keep returning back to */
    /* the host so we at least don't hang the machine. */
    goto loop;
}
