#
# (C) P.Horton 2004,2005,2006
#
# $Id: Makefile 186 2006-01-17 23:03:58Z pdh $
#
# This code is covered by the GNU General Public License. For details see the file "COPYING".
#

TARG= stage2
TOOLDIR= ../tools
OBJS=	main.o\
		lib.o\
		libmem.o\
		vsprintf.o\
		serial.o\
		shell.o\
		script.o\
		expr.o\
		edit.o\
		history.o\
		memory.o\
		srec.o\
		ide.o\
		block.o\
		ext2.o\
		md5.o\
		pci.o\
		cache.o\
		elf32.o\
		elf64.o\
		exec.o\
		inflate.o\
		tulip.o\
		keymap.o\
		flash.o\
		heap.o\
		launch.o\
		net.o\
		arp.o\
		ip.o\
		icmp.o\
		udp.o\
		dhcp.o\
		tftp.o\
		lcd.o\
		env.o\
		boot.o\
		nv.o\
		nfs.o\
		netcon.o\
		start.o\

include ../Rules.mak

DEPS=$(OBJS:.o=.d)

LDFLAGS= -nostdlib -Wl,-T,$(TARG).lnk,-Map,$(TARG).map,--emit-relocs
CFLAGS= -Werror -Wall -Wstrict-prototypes -fomit-frame-pointer -O2 -fno-strict-aliasing $(CFLAGS_CPU) $(CFLAGS_COLO)
CPPFLAGS= -MD -nostdinc -Iinclude -I../include -D_DEBUG $(CPPFLAGS_GCC)

LOADADDR= $(shell $(CROSS_COMPILE)readelf -l $(TARG) | awk '$$1 ~ /^LOAD$$/ { print $$3 }')

all:
	$(MAKE) -C .. binary

binary: $(TARG).elf

$(TARG).bin: $(TARG)
	$(OBJCOPY) -O binary $< $@

$(TARG)-wrap.S: $(TARG).rfx
	echo -e '\t.section .stage2\n\t.incbin "$<"' > $@

$(TARG).elf: $(TARG)-wrap.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARG).rfx: $(TARG)
	$(TOOLDIR)/elf2rfx/elf2rfx -f $< $@

$(TARG).tmp: $(TARG)
	$(TOOLDIR)/elf2rfx/elf2rfx -fb $(LOADADDR) $< $@

test-rfx: $(TARG).bin $(TARG).tmp
	cmp $^
	@echo '***  SUCCESS  ***'

clean:
	rm -f $(TARG){,.elf,.rfx,.map,.bin,.tmp,-wrap.S} $(OBJS) $(DEPS)

$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: src/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

%.o: src/%.S
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

-include $(DEPS)

.PHONY: all binary clean test-rfx

# vi:set ts=3 sw=3 ai:
