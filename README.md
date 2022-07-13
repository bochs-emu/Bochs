# Welcome to the Bochs IA-32 Emulator Project

Bochs is a portable IA-32 (x86) PC emulator written in C++,
that runs on most popular platforms. It includes emulation of the Intel x86 
CPU, common I/O devices, and a custom BIOS.

Bochs can be compiled to emulate many different x86 CPUs, from early 386 to
the most recent (sometimes even pre-market) x86-64 Intel and AMD processors.

Bochs is capable of running most Operating Systems inside the emulation 
including Linux, DOS or Microsoft Windows.

Bochs provides many different modes of operation, in support of a wide 
variety of use cases.  The 'typical' use of bochs is to provide complete 
x86 PC emulation, including the x86 processor, hardware devices, and memory.
This allows you to run OS's and software within the emulator on your workstation,
much like you have a machine inside of a machine. For instance, let's say 
your workstation is a Unix/X11 workstation, but you want to run Win'95 
applications. Bochs will allow you to run Win 95 and associated software on
your Unix/X11 workstation, displaying a window on your workstation, simulating
a monitor on a PC.

## Bochs Approach
Bochs is an emulator - not virtualization software.  It is portable across many 
architectures: x86, ARM, MIPS, etc.  This means it must be able to emulate 
every CPU instruction.

This distinguishes Bochs from virtualization solutions like e.g. VirtualBox,
VMWare, etc.  Those projects provide a nice user experience and fast
performance, at the cost of hardware constraints, some non-determinism and 
some necessary hacks to get programs working.

Bochs' emulation provides a controlled, accurate execution environment, at 
the cost of speed/performance.  This can be advantageous in some situations,
for example:
* When developing an operating system or bootloader
* When dealing with very old, mission-critical software
* When reverse-engineering system-level code

For more information, see [the intro section](https://bochs.sourceforge.io/cgi-bin/topper.pl?name=New+Bochs+Documentation&url=https://bochs.sourceforge.io/doc/docbook/user/index.html) in the user guide.

## Installing

You can download Bochs from the [project page on SourceForge](https://sourceforge.net/projects/bochs/files/bochs/2.7/). See the
CHANGES file for details on the most recent releases.

## Usage
See [the documentation](https://bochs.sourceforge.io/cgi-bin/topper.pl?name=New+Bochs+Documentation&url=https://bochs.sourceforge.io/doc/docbook/).

## Contributing
To get started, see [Bochs Developer Guide](https://bochs.sourceforge.io/cgi-bin/topper.pl?name=New+Bochs+Documentation&url=https://bochs.sourceforge.io/doc/docbook/).

We currently need help with the below tasks.  To help with one of these tasks, please contact Volker Ruppert or Stanislav Shwartsman.

### Bug Reports
Mouse, interrupt controller, timer, IDE controller, network 
card, keyboard, VGA... Most of our bug reports and feature requests are due
to incomplete C++ models of the various PC devices. To improve this, we 
need PC Hardware Gurus who know where to find the specs for this stuff and
improve the hardware models for Bochs. Working on models is a fun way to 
learn how things work, and unlike designing a real hard disk, you can test
out your changes on a real operating system immediately!

### Disk Images 
Our collection of disk images is getting out of date. It would be great to 
have small or large images of a variety of free operating systems.

### Documentation 
Adding installation help and other useful information into the docs.


## Papers/Presentations

* Bochs was presented at ISCA-35 in Beijing, China at "The 1st Workshop on 
Architectural and Microarchitectural Support for Binary Translation" by a 
paper "Virtualization without direct execution - designing a portable VM".
  * [Paper](https://bochs.sourceforge.io/Virtualization_Without_Hardware_Final.pdf)
  * [Slides](http://bochs.sourceforge.net/VirtNoJit.pdf)

## Authors
Bochs was originally written by Kevin Lawton and is currently maintained by this project.
