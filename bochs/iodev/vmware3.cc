/*
 * This file provides support for VMWare's virtual disk image
 * format.
 *
 * Author: Sharvil Nanavati, for Net Integration Technologies, Inc.
 * Contact: snrrrub@yahoo.com
 *  
 * Copyright (C) 2003 Net Integration Technologies, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "bochs.h"

/* Not very friendly... */
extern bx_hard_drive_c *theHardDrive;
#define LOG_THIS theHardDrive->

bool vmware3_image_t::is_valid_header(COW_Header & header)
{
    if(header.id[0] != 'C' || header.id[1] != 'O' || header.id[2] != 'W' ||
            header.id[3] != 'D')
    {
        BX_DEBUG(("not a vmware3 COW disk"));
        return false;
    }

    if(header.header_version != 3)
    {
        BX_DEBUG(("unsupported vmware3 COW disk header version"));

        return false;
    }

    if(header.vmware_version != 2)
    {
        BX_DEBUG(("unsupported vmware3 COW disk version"));
        return false;
    }

    return true;
}

char * vmware3_image_t::generate_cow_name(const char * filename, unsigned chain)
{
    char * name = new char[strlen(filename) + 4];
    if(name == 0)
        BX_PANIC(("unable to allocate %d bytes for vmware3 COW file name (base: %s, chain: %d)", strlen(filename) + 4, filename, chain));
    strcpy(name, filename);
    if(chain != 0)
    {
        char * period = strrchr(name, '.');
        if (period != 0)
        {
            char temp[1024];
            strcpy(temp, period + 1);
            *period = 0;
            sprintf(name, "%s-%02d.%s", name, chain + 1, temp);
        }
        else
            sprintf(name, "%s-%02d", name, chain + 1);
    }
    return name;
}

/* 
 * This function will panic if errors occur when attempting to open an image
 * file. Now if only I could use exceptions to handle the errors in an elegant
 * fashion...
 */
int vmware3_image_t::open(const char * pathname)
{
    COW_Header header;
    int file;
    int flags = O_RDWR;
#ifdef O_BINARY
    flags |= O_BINARY;
#endif

    /* Open the virtual disk */
    file = ::open(pathname, flags);

    if(file < 0)
        return -1;

    /* Read the header */
    if(::read(file, &header, sizeof(COW_Header)) < 0)
        BX_PANIC(("unable to read vmware3 COW Disk header from file '%s'", pathname));

    /* Make sure it's a valid header */
    if(!is_valid_header(header))
        BX_PANIC(("invalid vmware3 COW Disk image"));

    ::close(file);

    tlb_size  = header.tlb_size_sectors * 512;
    slb_count = (1 << FL_SHIFT) / tlb_size;
    images = new COW_Image [header.number_of_chains];

    off_t offset = 0;
    for (int i = 0; i < header.number_of_chains; ++i)
    {
        char * filename = generate_cow_name(pathname, i);
        current = &images[i];

        current->fd = ::open(filename, flags);
        if(current->fd < 0)
            BX_PANIC(("unable to open vmware3 COW Disk file '%s'", filename));

        if(::read(current->fd, &current->header, sizeof(COW_Header)) < 0)
            BX_PANIC(("unable to read header or invalid header in vmware3 COW Disk file '%s'", filename));

        if(!is_valid_header(current->header))
            BX_PANIC(("invalid vmware3 COW Disk file '%s'", filename));

        current->flb = new unsigned [current->header.flb_count];
        if(current->flb == 0)
            BX_PANIC(("cannot allocate %d bytes for flb in vmware3 COW Disk '%s'", current->header.flb_count * 4, filename));
        
        current->slb = new unsigned * [current->header.flb_count];
        if(current->slb == 0)
            BX_PANIC(("cannot allocate %d bytes for slb in vmware3 COW Disk '%s'", current->header.flb_count * 4, filename));
        
        for(int j = 0; j < current->header.flb_count; ++j)
        {
            current->slb[j] = new unsigned [slb_count];
            if(current->slb[j] == 0)
                BX_PANIC(("cannot allocate %d bytes for slb[] in vmware3 COW Disk '%s'", slb_count * 4, filename));
        }

        current->tlb = new char [tlb_size];
        if(current->tlb == 0)
            BX_PANIC(("cannot allocate %d bytes for tlb in vmware3 COW Disk '%s'", tlb_size, filename));
        
        if(::lseek(current->fd, current->header.flb_offset_sectors * 512, SEEK_SET) < 0)
            BX_PANIC(("unable to seek vmware3 COW Disk file '%s'", filename));

        if(::read(current->fd, (char*)current->flb, current->header.flb_count * 4) < 0)
            BX_PANIC(("unable to read flb from vmware3 COW Disk file '%s'", filename));

        for(int j = 0; j < current->header.flb_count; ++j)
            if(current->flb[j] != 0)
            {
                if(::lseek(current->fd, current->flb[j] * 512, SEEK_SET) < 0)
                    BX_PANIC(("unable to seek vmware3 COW Disk file '%s'", filename));
                if(::read(current->fd, (char*)current->slb[j], slb_count * 4) < 0)
                    BX_PANIC(("unable to read slb from vmware3 COW Disk file '%s'", filename));
            }

        current->min_offset = offset;
        offset += current->header.total_sectors * 512;
        current->max_offset = offset;

        current->offset = INVALID_OFFSET;
		current->synced = true;
        delete[] filename;
    }
    current = &images[0];
    requested_offset = 0;
    cylinders = header.cylinders_in_disk;
    heads = header.heads_in_disk;
    sectors = header.sectors_in_disk;
    return 1;
}

off_t vmware3_image_t::perform_seek()
{
	if(requested_offset < current->min_offset || requested_offset >= current->max_offset)
	{
		if(!sync())
		{
			BX_DEBUG(("could not sync before switching vmware3 COW files"));
			return INVALID_OFFSET;
		}

	    while(requested_offset < current->min_offset)
    	    current = &images[current->header.chain_id - 1];

	    while(requested_offset >= current->max_offset)
    	    current = &images[current->header.chain_id + 1];
	}

    if(current->offset != INVALID_OFFSET && requested_offset >= current->offset
            && requested_offset < current->offset + tlb_size)
        return (requested_offset - current->offset);

	if(!sync())
	{
		BX_DEBUG(("could not sync before seeking vmware3 COW file"));
		return INVALID_OFFSET;
	}

    unsigned relative_offset = (requested_offset - current->min_offset);
    unsigned i = relative_offset >> FL_SHIFT;
    unsigned j = (relative_offset & ~FL_MASK) / tlb_size;

    if(current->slb[i][j])
    {
        if(::lseek(current->fd, current->slb[i][j] * 512, SEEK_SET) < 0)
        {
            BX_DEBUG(("could not seek vmware3 COW to sector slb[%d][%d]", i, j));
            return INVALID_OFFSET;
        }
        if(::read(current->fd, current->tlb, tlb_size) < 0)
        {
            BX_DEBUG(("could not read %d bytes from vmware3 COW image", tlb_size));
            return INVALID_OFFSET;
        }
    }
    else
        memset(current->tlb, 0, tlb_size);

    current->offset = (requested_offset / tlb_size) * tlb_size;
    return (requested_offset - current->offset);
}

ssize_t vmware3_image_t::read(void * buf, size_t count)
{
    ssize_t total = 0;
    while(count > 0)
    {
        off_t offset = perform_seek();
        if(offset == INVALID_OFFSET)
        {
            BX_DEBUG(("vmware3 COW read failed on %d bytes", count));
            return -1;
        }
        unsigned bytes_remaining = tlb_size - offset;
        unsigned amount = (bytes_remaining > count) ? count : bytes_remaining;
        memcpy(buf, current->tlb + offset, amount);
        requested_offset += amount;
        total += amount;
        count -= amount;
    }
    return total;
}

/* This could be done much better, I'm sure. In fact, the whole header doesn't
 * need to be re-written each time a new tlb is allocated nor does the whole
 * slb need to be re-written (most of the time) but that can be changed whenever
 * it becomes an issue... image I/O is not a bottleneck.
 */
bool vmware3_image_t::sync()
{
	if(current->synced)
		return true;

    unsigned relative_offset = (current->offset - current->min_offset);
    unsigned i = relative_offset >> FL_SHIFT;
    unsigned j = (relative_offset & ~FL_MASK) / tlb_size;
    
    if (current->slb[i][j] == 0)
    {
        if (current->flb[i] == 0)
        {
            unsigned slb_size = slb_count * 4;
            
            /* Re-write the FLB */
            current->flb[i] = current->header.next_sector_to_allocate;
            if(::lseek(current->fd, current->header.flb_offset_sectors * 512, SEEK_SET) < 0)
            {
                BX_DEBUG(("could not seek vmware3 COW image to flb on sync"));
                return false;
            }
            if(::write(current->fd, current->flb, current->header.flb_count * 4) < 0)
            {
                BX_DEBUG(("could not re-write flb to vmware3 COW image on sync"));
                return false;
            }
            current->header.next_sector_to_allocate += (slb_size / 512) + ((slb_size % 512) ? 1 : 0);
        }

        /* Re-write the SLB */
        current->slb[i][j] = current->header.next_sector_to_allocate;
        if(::lseek(current->fd, current->flb[i] * 512, SEEK_SET) < 0)
        {
            BX_DEBUG(("could not seek vmware3 COW image to slb on sync"));
            return false;
        }
        if(::write(current->fd, current->slb[i], slb_count * 4) < 0)
        {
            BX_DEBUG(("could not re-write slb to vmware3 COW image on sync"));
            return false;
        }
        current->header.next_sector_to_allocate += current->header.tlb_size_sectors;

        /* Update the header */
        if(::lseek(current->fd, 0, SEEK_SET) < 0)
        {
            BX_DEBUG(("could not seek to vmware3 COW image to offset 0 on sync"));
            return false;
        }
        if(::write(current->fd, &current->header, sizeof(COW_Header)) < 0)
        {
            BX_DEBUG(("could not re-write header to vmware3 COW image on sync"));
            return false;
        }
    }
    if(::lseek(current->fd, current->slb[i][j] * 512, SEEK_SET) < 0)
    {
        BX_DEBUG(("could not seek vmware3 COW image to offset %d on sync", current->slb[i][j] * 512));
        return false;
    }
    if(::write(current->fd, current->tlb, tlb_size) < 0)
    {
        BX_DEBUG(("could not write tlb to vmware3 COW image on sync"));
        return false;
    }
	current->synced = true;
    return true;
}

ssize_t vmware3_image_t::write(const void * buf, size_t count)
{
    ssize_t total = 0;
    while(count > 0)
    {
        off_t offset = perform_seek();
        if(offset == INVALID_OFFSET)
            return -1;
        unsigned bytes_remaining = tlb_size - offset;
        unsigned amount = 0;
		current->synced = false;
        if(bytes_remaining > count)
        {
            memcpy(current->tlb + offset, buf, count);
            amount = count;
        }
        else
        {
            memcpy(current->tlb + offset, buf, bytes_remaining);
            if(!sync())
            {
                BX_DEBUG(("failed to sync when writing %d bytes", count));
                return -1;
            }
            amount = bytes_remaining;
        }
        requested_offset += amount;
        total += amount;
        count -= amount;
    }
    return total;
}

off_t vmware3_image_t::lseek(off_t offset, int whence)
{
    if(whence == SEEK_SET)
        requested_offset = offset;
    else if (whence == SEEK_CUR)
        requested_offset += offset;
    else if (whence == SEEK_END)
        requested_offset = (off_t)(current->header.total_sectors_in_disk * 512) + offset;
    else
    {
        BX_DEBUG(("unknown 'whence' value (%d) when trying to seek vmware3 COW image", whence));
        return -1;
    }
    return requested_offset;
}

void vmware3_image_t::close()
{
    if(current == 0)
        return;

    unsigned count = current->header.number_of_chains;
    for(int i = 0; i < count; ++i)
    {
        current = &images[i];
        for(int j = 0; j < current->header.flb_count; ++j)
            delete[] current->slb[j];
        delete[] current->flb;
        delete[] current->slb;
        delete[] current->tlb;
        ::close(current->fd);
    }
    delete[] images;
    current = 0;
}
