/////////////////////////////////////////////////////////////////////////
// $Id: keymap.cc,v 1.1 2001-12-14 17:54:58 cbothamy Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  Christophe Bothamy
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

/////////////////////////////////////////////////////////////////////////
//
// Todo
//  . Check if other guis need mapping.
//  . Tables look-up should be optimised.
//  . For now up to 10 different map can be uploaded to the object
//    The table size should be increased/dynamic.
//


#include "bochs.h"

bx_keymap_c bx_keymap;

#define LOG_THIS bx_keymap.

bx_keymap_c::bx_keymap_c(void)
{
    put("KMAP");
    nbKeymaps=0;
    defaultKeymap=BX_KEYMAP_MAX_TABLES;
}

bx_keymap_c::~bx_keymap_c(void)
{
}


    void
bx_keymap_c::addTable(const char *mapName, const void *keymap, Bit32u size)
{
    if(nbKeymaps<BX_KEYMAP_MAX_TABLES) {
        keymaps[nbKeymaps]=keymap;
        mapNames[nbKeymaps]=mapName;
        mapSizes[nbKeymaps]=size;

        BX_INFO(("Added keymap '%s' at position %d (%d entries)",mapName,nbKeymaps,size));

        nbKeymaps++;
        }
    else {
        BX_PANIC(("No space to add keymap '%s'",mapName));
    }
}

    void
bx_keymap_c::setDefault(const char *mapName)
{
    Bit32u i;

    for(i=0;i<nbKeymaps;i++){
        if(strcmp(mapName,mapNames[i])==0){
            defaultKeymap=i;
         BX_INFO(("Default keymap set to '%s'",mapNames[i]));
         return;
        }
    }
    BX_ERROR(("Unknown keymap %s",mapName));
}

    void*
bx_keymap_c::getTable(char *mapName)
{
    Bit32u i;

    for(i=0;i<nbKeymaps;i++){
        if(strcmp(mapName,mapNames[i])==0){
            return(&keymaps[i]);
        }
    }
    BX_ERROR(("Unknown keymap %s",mapName));
}

    void*
bx_keymap_c::getTable(void)
{
    char* localKeymap;

    localKeymap=bx_options.keyboard.Okeymap->getptr();
    return(getTable(localKeymap));
}

    Bit32u
bx_keymap_c::getBXKey(char* mapName, Bit32u key)
{
    Bit32u i,mapIndex;
    Bit32u *keymap,size;

    // First we browse through the specific keymap
    mapIndex=BX_KEYMAP_MAX_TABLES;
    for(i=0; i<nbKeymaps; i++){
            if(strcmp(mapName,mapNames[i])==0){
                    mapIndex=i;
                    break;
                    }
            }
    if(mapIndex<BX_KEYMAP_MAX_TABLES){
    keymap=(Bit32u*)(keymaps[mapIndex]);
    size=mapSizes[mapIndex];
    for(i=0; i<size; i++){
        if(keymap[i*2]==key)
            return(keymap[i*2+1]);
        }
    }
    else BX_ERROR(("Unknown keymap %s",mapName));

    // Then, if not found, search into the default keymap
    if((defaultKeymap<BX_KEYMAP_MAX_TABLES)
     &&(defaultKeymap!=mapIndex)){
    keymap=(Bit32u*)keymaps[defaultKeymap];
    size=mapSizes[defaultKeymap];
    for(i=0;i <size; i++){
        if(keymap[i*2]==key)
            return(keymap[i*2+1]);
        }
    }

    // Return default
    return BX_KEY_UNHANDLED;
}

    Bit32u
bx_keymap_c::getBXKey(Bit32u key)
{
    Bit32u i,mapIndex;
    Bit32u *keymap,size;
    char* localKeymap;

    localKeymap=bx_options.keyboard.Okeymap->getptr();

    return(getBXKey(localKeymap,key));
}

