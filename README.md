Unnamed GameBoy Advance demo
============================

Build instructions
------------------
The demo can be compiled with any GCC/binutils `arm-none-eabi-` cross-toolchain.
There is no dependency to any specific devkit or external library; the source
tree is self-contained. You can however use devkitARM as the source of your
cross-toolchain, if that's already installed, or easier to get.

On Debian GNU/Linux the easiest way to install the required toolchain is to run:
    apt-get install gcc-arm-none-eabi binutils-arm-none-eabi gdb-multiarch

To build the demo, just type `make`.

`make disasm` will produce a disassembly of the whole binary.

`make run` will attempt to run it with the mgba emulator.

Build options can be set by creating a `cfg.mk` file in the project directory.

For example to enable emulator build (with emulator printf enabled), create a
`cfg.mk` file with the contents: `def += -DEMUBUILD`. Or to enable the vblank
interrupt performance bars, set `def += -DVBLBAR`.

Debugging
---------
mgba includes a gdbserver. Run `make debug` to automatically start mgba in gdb
debug mode, and connect gdb to it. You can set breakpoints, examine state, and
so on as usual.
