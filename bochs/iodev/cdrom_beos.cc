/////////////////////////////////////////////////////////////////////////
// $Id: cdrom_beos.cc,v 1.5 2009-02-08 09:05:52 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

#include <SupportDefs.h>
#include <Drivers.h>

#include <sys/stat.h>
#include <unistd.h>

#include "cdrom_beos.h"

int GetLogicalBlockSize(int fd)
{
    partition_info p_info;

    if (ioctl(fd, B_GET_PARTITION_INFO, &p_info) == B_NO_ERROR)
    {
        //dprintf("GetLogicalBlockSize: ioctl suceed\n");
        return p_info.logical_block_size;
    }
    else //dprintf("GetLogicalBlockSize = ioctl returned error\n");
        return 0;
}

int GetDeviceBlockSize(int fd)
{
    struct stat     st;
    device_geometry dg;

    if (ioctl(fd, B_GET_GEOMETRY, &dg) < 0)
    {
        if (fstat(fd, &st) < 0 || S_ISDIR(st.st_mode))
            return 0;
        return 512;   /* just assume it's a plain old file or something */
    }

    return dg.bytes_per_sector;
}

off_t GetNumDeviceBlocks(int fd, int block_size)
{
    struct stat        st;
    device_geometry dg;

    if (ioctl(fd, B_GET_GEOMETRY, &dg) >= 0)
    {
        return (off_t)dg.cylinder_count *
               (off_t)dg.sectors_per_track *
               (off_t)dg.head_count;
    }

    /* if the ioctl fails, try just stat'ing in case it's a regular file */
    if (fstat(fd, &st) < 0)
        return 0;

    return st.st_size / block_size;
}
