i440fx - A BIOS for Bochs and QEMU
Assembly source code available at https://github.com/fysnet/i440fx

    Contents

  * Overview: An overview of this BIOS
  * Objective: The objective of this BIOS
  * Resources: The required resources for this BIOS
  * Building: Building this BIOS from source
  * Usage: Using this BIOS with Bochs or QEMU
  * User interaction: Interaction with the BIOS
  * ESCD: Extended System Configuration Data usage
  * USB: USB emulation
  * Known issues: A list of known issues
  * Guests: A list of Guests I have tested with
  * Post: Last words


    Overview

  * This BIOS is designed to emulate the Intel 440fx (Pentium, 
    Pentium Pro, and Pentium II) and the later Intel 440bx
    (Pentium II, Pentium III, and Celeron) chipsets.
  * i440fx is an open source adaptation of the original Bochs BIOS
    with many fixes and enhancements. It is written entirely in x86
    Assembly assembled with the NBASM assembler.
  * It is designed to boot a guest following the BIOS Boot Specification
    with enhancements like PnP, USB emulation, ACPI, and other resources.
  * Unlike the previous source, this adaptation requires only one build
    application (the assembler) with a single command line. No
    manipulation or preprocessing of source code and no post-processing.
    A single assembler and a single command line creates the BIOS image
    all at once.
  * This BIOS is designed for and has been tested to boot guests as old
    as IBM PCDOS 2.0 (as long as you don't use the internal BASIC
    interpreter such as with BASICA, etc.) to most modern quests such as
    Windows 11 and various unix based quests.


    Objective

  * The objective for this BIOS was first to see if I can do it. I
    wanted to see if I could adapt and enhance the given BIOS to boot a
    wider range of guests, and second, to add booting from USB devices.
  * There are a few errors with the previous BIOS, a few in the CD-ROM
    emulation, as well as other misc errors. The objective of this BIOS
    is to eliminate errors and enhance its function.


    Resources

  * This BIOS is designed to be used with the BOCHS emulator.
      o Bochs Project page: https://sourceforge.net/projects/bochs/files/bochs/
      o Bochs github source: https://github.com/bochs-emu/Bochs
  * The source code for this BIOS.
      o i440fx: https://github.com/fysnet/i440fx
      o This source is licensed as:
              *                                                                          *
              *  Copyright (c) 1984-2025    Forever Young Software  Benjamin David Lunt  *
              *                                                                          *
              *                         i440FX BIOS ROM v1.0                             *
              *                                                                          *
              * This code is freeware, not public domain.  Please use respectfully.      *
              *                                                                          *
              * You may:                                                                 *
              *  - use this code for learning purposes only.                             *
              *  - use this code in your own Operating System development.               *
              *  - distribute any code that you produce pertaining to this code          *
              *    as long as it is for learning purposes only, not for profit,          *
              *    and you give credit where credit is due.                              *
              *                                                                          *
              * You may NOT:                                                             *
              *  - distribute this code for any purpose other than listed above.         *
              *  - distribute this code for profit.                                      *
              *                                                                          *
              * You MUST:                                                                *
              *  - include this whole comment block at the top of this file.             *
              *  - include contact information to where the original source is located.  *
              *            https://github.com/fysnet/i440fx                              *
              *                                                                          *
                    

    Building

  * This BIOS is assembled with the NewBasic assembler.
      o NewBasic Assembler: https://www.fysnet.net/newbasic.htm -- version 00.27.16 or higher

  * This assembler is intended for DOS and 32-bit and 64-bit Windows.
    However, using additional resources, you can use this assembler on
    Linux.

      o Windows (Command Prompt):

        To build this BIOS for BOCHS, from the i440fx directory, use the
        following commandline:

               NBASM i440fx /x /z

        This will produce |i44fx.bin| ready to be inserted into the
        emulation. It will also create a |.lst| file showing all of the
        references throughout the source code, useful for debugging.

        To build this BIOS for Qemu, from the i440fx directory, use the
        following commandline:

               NBASM i440fx /x /z /DBX_QEMU

        You can exclude the |/x| parameter if you wish. If you exclude
        the |/z| parameter, NBASM will show a few diagnostic warnings.
        These can be ignored.

      o Linux:

        Using additional resources, you can use NBASM on Linux.

        I don't use Linux but a fellow user says you can use WINE to run
        NBASM. Just ignore the warning at the start and the end of the
        Wine output.

        To build this BIOS for BOCHS, from the i440fx directory, use the
        following:

               wine /path/to/program/nbasm64.exe /x /z i440fx

        This will produce |i44fx.bin| ready to be inserted into the
        emulation. It will also create a |.lst| file showing all of the
        references throughout the source code useful for debugging.

        To build this BIOS for Qemu, from the i440fx directory, use the
        following:

               wine /path/to/program/nbasm64.exe /x /z /DBX_QEMU i440fx

        You can exclude the |/x| parameter if you wish. If you exclude
        the |/z| parameter, NBASM will show a few diagnostic warnings.
        These can be ignored.


    Using

  * To use this BIOS with Bochs, place one of the following lines in
    your |bochsrc.txt| file:
      o  romimage: file=$BXSHARE/bios/i440fx/i440fx.bin
      o  romimage: file=$BXSHARE/bios/i440fx/i440fx.bin, flash_data="escd.bin"

    If you haven't set the |$BXSHARE| parameter to point to the Bochs
    directory, change the above to point directly to the binary file.

    The second example will use the ESCD (Extended System Configuration
    Data) data given in the |escd.bin| file. See the ESCD section for more 
    on this. If the file doesn't exist, this BIOS will create one using defaults.

    It is recommended that you use the following addition:
      o  cpu: model=pentium

  * To use this BIOS with QEMU, use the following parameter:
      o  -bios C:\path\to\this\bios\i440fx_qemu.bin

    It is recommended that you use the following addition:
      o  -cpu pentium3-v1

    The following has been known to work, but not completely supported:
      o  -machine q35


    User Interaction

  * During Boot, you will see a prompt.

  * Depending on the settings, you will have just a few seconds to press
    either F10 or F12.

    If you press F10, the setup will be shown. See the ESCD section for 
    more on this.

    If you press F12, you will be shown a list of found bootable
    devices. Press the letter next to the device you wish to boot.


    ESCD (Extended System Configuration Data)

  * This BIOS uses the Extended System Configuration Data (ESCD) area in
    the 0xFC000-0xFFFFF range.
  
    If you include an ESCD file to use (see a previous section), this
    BIOS will use the contents of that file for settings and other data
    during boot time. If you did not include a file, defaults will be used.

    This file and the data within it is used for various items
    throughout the boot.

      o Num Lock: Shall the Num Lock state be on or off at boot time?
      o Delay: The count of seconds to delay waiting for a key press at
        the shown prompt.
      o Other items will be added here.

  
    The Setup page:

  * The setup page appears when you press F10 at the boot prompt.
  * You can use the mouse or the arrow keys to move around the items. If
    you use the arrow keys, the space key will simulate a mouse click on
    the item selected.
  * Please note that a lot of work is still needed for this setup page
    and I will update this documentation when that work progresses.


    USB Emulation

  * This BIOS will emulate a given USB device.
  * Currently only the following items are supported:
      o  USB Floppy Disk
      o  USB Hard Drive
      o  USB CD-ROM Disc
  * Future plans are to emulate a USB mouse and Keyboard.
    To boot a USB device, use the following in your |bochsrc.txt| file:

           boot: usb

    Then depending on the USB controller used:

           usb_uhci: port1=floppy, options1="speed:full, path:some/path/floppy.img, model:teac"

    To boot a floppy or other drive using another controller, use a very
    similar line:

           usb_ohci: port2=cdrom, options2="speed:full, path:../common/bootcd.iso"
           usb_ehci: port1=disk, options1="speed:high, path:../common/hdd.img, proto:bbb"
           usb_xhci: port1=disk, options1="speed:super, path:../common/hdd.img, proto:bbb"

    Remember that each controller other than the UHCI requires the |slotx=| 
    addition in the pci declaration:

           pci: enabled=1, chipset=i440fx, slot1=usb_ohci

  * The emulation will be on the first found USB device, or press F12 to
    choose one from a list.
  * The uasp protocol is currently not supported with this BIOS.

    
    Type of device detected:

    This BIOS will do a few calculations on a given USB image file
    determining how to boot said file.

      o  If a USB HDD device is detected, the following checks will be
         made on the first sector:
         1. byte[0] == 0xEB and byte[2] == 0x90 ; jmp short xxxx
          or
            byte[0] == 0xE9 and word[1] < 0x1FE ; jmp near xxxx
         2. word at [11] == 512 ; bytes per sector
         3. word at [14] > 0 ; reserved sectors
         4. byte at [16] == 1 or 2 ; number of fats
         5. bytes at [54:61] == 'FAT12 ' ; system
         6. word at [19] == 2880 ; sectors
          or
            word at [19] == 0 && dword at [32] == 2880 ; extended sectors

         If all of the above is true, no matter the size of the HDD
         image, this BIOS will mount it as a 1.44M floppy disk.

      o  If the Block Size == 2048, a CD-ROM is assumed and will attempt
         to mount the bootable image within the CD-ROM using:

         Depending on the media (Eltorito Boot included?)
          + no emulation using 2048-byte sectors
          + floppy or hdd emulation using 512-byte sectors.
      o  Otherwise, it will attempt to mount as a hdd device with or w/o
         a MBR.


    A list of known issues

  * This BIOS will print to the console, any panics or warnings indicating that something went wrong, or isn't supported.
    If currently in a text mode (like DOS), it will display this text at the current cursor location, possibly disturbing
     the guest's text output. Once more testing is done, these panics and warning will be removed.
  * This BIOS uses the |xchg cx,cx| instruction to stop the Bochs debugger at certain places, mostly places that have yet
     to be supported. Most of the time, you can simply continue.
  * As of right now, after a major re-build, you should delete the |escd.bin| file so that this BIOS will build a new
     default file. For example, the latest build retrieves the seconds to wait for a F12/F10 key press. If you have an
     old |escd.bin| file, it might return zero (or 0x90) and either not wait for you to press a key, or wait a long time.
  * When using Jemm386 (v5.79 and possibly all versions) and/or HimemX (v3.36 and possibly all versions), one or the other
     will remap the address range 0x000E0000 -> 0x000E3FFF to 0x00149000. Of course this will break this BIOS, or any BIOS
     that uses the extended BIOS area starting at 0x000E0000.
    Using standard (MS-)DOS memory extenders work as expected.
    Build with /DHIMEMHACK to allow the use of Jemm386 and/or HimemX. This will ignore the part that breaks this BIOS.
     See https://github.com/fysnet/i440fx/issues/4 for more information.


  A list of guests I have tested with

   * Note that even though I list a guest OS here, this doesn't mean my BIOS passes all tests. It simply means that I
      have successfully booted this guest with default or normal options and parameters. Thorough testing is still in order.
     If you have tested this BIOS with your homebrew OS or any other OS and would like it listed here, please let me know
      by sending me a URL to a bootable image. I can be reached at fys [at] fysnet [dot] net.

     (Listed in no particular order)
       o  FreeDOS (https://www.freedos.org/)
       o  Gentoo (older version, I haven't tried a latest release yet) (https://www.gentoo.org/)
       o  netbsd (older version, I haven't tried a latest release yet) (https://www.netbsd.org/)
       o  MS-DOS v6.22 (Other versions have been tested sporadically)
       o  OS2Warp v4.0
       o  pdos (http://pdos.org/)
       o  ReactOS v0.4.14 (https://reactos.org/)
       o  SliTaz (older version, I haven't tried a latest release yet) (https://slitaz.org/en/)
       o  TinyCore (older version, I haven't tried a latest release yet) (http://tinycorelinux.net/)
       o  Windows 3.11
       o  Windows 95
       o  Windows 95 OSR2
       o  Windows 98
       o  Windows 98 SE
       o  Windows 2000
       o  Windows ME
       o  Windows XP
       o  Windows 7
       o  Windows 8.1
       o  Windows 10
       o  Windows 11


    Conclusion

  * All comments, patches, fixes, or other are always welcome,
    especially if the intent is to enhance this BIOS. Please create a
    Pull Request or create an Issue if you wish to contribute.


Latest Update: 10 Feb 2025, 17.30
