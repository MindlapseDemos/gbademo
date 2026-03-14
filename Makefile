src = $(wildcard src/*.c) $(wildcard src/libc/*.c)
ssrc = $(wildcard src/*.s) $(wildcard src/libc/*.s)
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

#def =
opt = -O3 -fomit-frame-pointer -fcommon -mcpu=arm7tdmi -mtune=arm7tdmi -mthumb -mthumb-interwork
dbg = -g
inc = -nostdinc -Isrc/libc
warn = -pedantic -Wall -Wno-char-subscripts

CFLAGS = -std=gnu99 $(opt) $(dbg) $(warn) -MMD $(def) $(inc)
ASFLAGS = -mthumb-interwork
LDFLAGS = -mthumb -mthumb-interwork -nostdlib -T gbademo.ld -lgcc $(libs) -lm

-include cfg.mk

.PHONY: all
all: $(bin) $(bin_mb)

$(bin): $(elf)
	$(OBJCOPY) -O binary $(elf) $(bin)
	gbafix -r0 $(bin)

$(elf): $(obj) $(libs)
	$(CC) -o $(elf) $(obj) -Wl,-Map,link.map $(LDFLAGS)

-include $(dep)

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

.PHONY: install
install: $(bin)
	if2a -n -f -W $<

.PHONY: run
run: $(bin)
	mgba -3 --log-level=16 $(bin)

.PHONY: debug
debug: $(elf)
	mgba -2 -g $(bin) &
	$(TCPREFIX)gdb -x gdbmgba $<

.PHONY: disasm
disasm: $(elf)
	$(OBJDUMP) -d $< >$@
