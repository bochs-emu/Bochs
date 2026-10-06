This is a fork of the Bochs emulator.

I've added the ability to control the state of the A20 line before the system's bootloader takes over.

This mechanism is BIOS-independent; meaning it works with any BIOS (BIOS-bochs-legacy, BIOS-bochs-latest, SeaBIOS, etc.).

The user can now write the following in the resources file (bochsrc):
```
A20: enable=0
```
This will disable the A20 line before handing control over to the bootloader.

If this line is absent or the value of the "enable" parameter is 1, Bochs will enable line A20 (which is the default behavior).

For now the A20 line can be disabled if Bochs is running with 1 CPU only, or the built-in debugger is running.
