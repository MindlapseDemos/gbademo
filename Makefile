src = $(wildcard src/*.c) $(wildcard src/g3d/*.c) $(wildcard src/libc/*.c)
ssrc = $(wildcard src/*.s) $(wildcard src/g3d/*.s) $(wildcard src/libc/*.s)
obj = $(src:.c=.o) $(ssrc:.s=.o)
dep = $(src:.c=.d)
name = gbademo
elf = $(name).elf
bin = $(name).gba

#data = 

TCPREFIX = arm-none-eabi-

CPP = $(TCPREFIX)cpp
CC = $(TCPREFIX)gcc
AS = $(TCPREFIX)as
OBJCOPY = $(TCPREFIX)objcopy
OBJDUMP = $(TCPREFIX)objdump

arch = -mcpu=arm7tdmi -mthumb
#def =
#opt = -O3 -fomit-frame-pointer
dbg = -g
inc = -nostdinc -Isrc -Isrc/libc
warn = -pedantic -Wall -Wno-char-subscripts

CFLAGS = -std=gnu99 $(arch) $(opt) $(dbg) $(warn) -MMD $(def) $(inc)
ASFLAGS = $(arch)
LDFLAGS = $(arch) -nostdlib -T gbademo.ld -Wl,--gc-sections -Wl,-Map,link.map -lgcc $(libs)

-include cfg.mk

.PHONY: all
all: $(bin) $(bin_mb)

$(bin): $(elf)
	$(OBJCOPY) -O binary $(elf) $(bin)
	gbafix -r0 $(bin)

$(elf): $(obj) $(libs)
	$(CC) -o $(elf) $(obj) $(LDFLAGS)

-include $(dep)

%.s: %.c
	$(CC) -o $@ -S $< $(CFLAGS) -g0
	mv $@ foo.s

src/data.o: src/data.s $(data)

tools/imgquant/imgquant:
	$(MAKE) -C tools/imgquant

%.sraw: %.png tools/imgquant/imgquant
	tools/imgquant/imgquant -o $@ -oc $(subst .sraw,.spal,$@) -os $(subst .sraw,.shade,$@) -s 8 $<

%.raw: %.png tools/imgquant/imgquant
	tools/imgquant/imgquant -o $@ -n $<

%.pal: %.png tools/imgquant/imgquant
	tools/imgquant/imgquant -o $@ -c $<

%.gpal: %.png tools/imgquant/imgquant
	tools/imgquant/imgquant -o $@ -c -g $<

%.555: %.png tools/imgquant/imgquant
	tools/imgquant/imgquant -o $@ -555 $<

data/lut.s: tools/lutgen
	tools/lutgen >$@

data/snd.bin: $(audata) tools/mmutil/mmutil
	tools/mmutil/mmutil -o$@ -hdata/snd.h $(audata)

.PHONY: clean
clean:
	rm -f $(obj) $(bin) $(bin_mb) $(elf) $(elf_mb)

.PHONY: cleandep
cleandep:
	rm -f $(dep)

.PHONY: deepclean
deepclean:
	find src -name '*.o' | xargs rm
	find src -name '*.d' | xargs rm
	rm *.elf *.gba

.PHONY: install
install: $(bin)
	if2a -n -f -W $<

.PHONY: run
run: $(bin)
	mgba -3 --log-level=16 $(bin)

.PHONY: debug
debug: $(elf)
	mgba -2 -g $(bin) &
	gdb-multiarch $<
#	$(TCPREFIX)gdb $<

.PHONY: disasm
disasm: $(elf)
	$(OBJDUMP) -d $< >$@
