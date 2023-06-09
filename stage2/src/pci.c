/*
 * (C) P.Horton 2004,2005,2006
 *
 * $Id: pci.c 186 2006-01-17 23:03:58Z pdh $
 *
 * This code is covered by the GNU General Public License. For details see the file "COPYING".
 */

#include "lib.h"
#include "cpu.h"
#include "galileo.h"
#include "pci.h"

/* Galileo generates special cycles on access to device 31 */

#define PCI_DEVICES_MAX				31

unsigned cp0_count_freq = CP0_COUNT_RATE_DEFAULT;

static unsigned unit;

static void fixup_count_rate(void)
{
	unsigned count;

	BRDG_REG_WORD(BRDG_REG_COUNTER_CTRL) = 0;

	BRDG_REG_WORD(BRDG_REG_COUNTER_0) = (BRDG_TCK + 10) / 20;

	BRDG_REG_WORD(BRDG_REG_COUNTER_CTRL) = (1 << 0);

	count = MFC0(CP0_COUNT);

	while(BRDG_REG_WORD(BRDG_REG_COUNTER_0))
		;

	count = MFC0(CP0_COUNT) - count;

	BRDG_REG_WORD(BRDG_REG_COUNTER_CTRL) = 0;

	cp0_count_freq = (count + 500) / 1000 * 20 * 1000;
}

unsigned cpu_clock_khz(void)
{
	return (cp0_count_freq * 2 + 500) / 1000;
}

void pcicfg_write_word(unsigned dev, unsigned func, unsigned addr, unsigned data)
{
	assert(!(addr & 3));

	BRDG_REG_WORD(0xcf8) = 0x80000000 | (dev << 11) | (func << 8) | addr;
	BRDG_REG_WORD(0xcfc) = data;
}

void pcicfg_write_half(unsigned dev, unsigned func, unsigned addr, unsigned data)
{
	assert(!(addr & 1));

	BRDG_REG_WORD(0xcf8) = 0x80000000 | (dev << 11) | (func << 8) | addr;
	BRDG_REG_HALF(0xcfc | (addr & 3)) = data;
}

void pcicfg_write_byte(unsigned dev, unsigned func, unsigned addr, unsigned data)
{
	BRDG_REG_WORD(0xcf8) = 0x80000000 | (dev << 11) | (func << 8) | addr;
	BRDG_REG_BYTE(0xcfc | (addr & 3)) = data;
}

unsigned pcicfg_read_word(unsigned dev, unsigned func, unsigned addr)
{
	assert(!(addr & 3));

	BRDG_REG_WORD(0xcf8) = 0x80000000 | (dev << 11) | (func << 8) | addr;
	return BRDG_REG_WORD(0xcfc);
}

unsigned pcicfg_read_half(unsigned dev, unsigned func, unsigned addr)
{
	assert(!(addr & 1));

	BRDG_REG_WORD(0xcf8) = 0x80000000 | (dev << 11) | (func << 8) | addr;
	return BRDG_REG_HALF(0xcfc | (addr & 3));
}

unsigned pcicfg_read_byte(unsigned dev, unsigned func, unsigned addr)
{
	BRDG_REG_WORD(0xcf8) = 0x80000000 | (dev << 11) | (func << 8) | addr;
	return BRDG_REG_BYTE(0xcfc | (addr & 3));
}

unsigned pci_unit_id(void)
{
	return unit;
}

const char *pci_unit_name(void)
{
	static const char *name[] = {
		[UNIT_ID_QUBE1]	= "Qube",
		[UNIT_ID_RAQ1]		= "RaQ",
		[UNIT_ID_QUBE2]	= "Qube2",
		[UNIT_ID_RAQ2]		= "RaQ2",
	};
	static char buf[16];

	if(unit >= elements(name) || !name[unit]) {
		sprintf(buf, "#%u", unit);
		return buf;
	}

	return name[unit];
}

void pci_init(size_t bank0, size_t bank1)
{
	/* set Galileo BARs for bus master memory access */

	BRDG_REG_WORD(BRDG_REG_RAS01_BANK_SIZE) = bank0 ? bank0 - 1 : 0;
	BRDG_REG_WORD(BRDG_REG_RAS23_BANK_SIZE) = bank1 ? bank1 - 1 : 0;

	if(bank0 >= bank1) {
		pcicfg_write_word(PCI_DEV_GALILEO, PCI_FNC_GALILEO, 0x10, 0);
		pcicfg_write_word(PCI_DEV_GALILEO, PCI_FNC_GALILEO, 0x14, bank0);
	} else {
		pcicfg_write_word(PCI_DEV_GALILEO, PCI_FNC_GALILEO, 0x10, bank1);
		pcicfg_write_word(PCI_DEV_GALILEO, PCI_FNC_GALILEO, 0x14, 0);
	}

	/* enable Galileo as bus master, and enable memory accesses */

	pcicfg_write_half(PCI_DEV_GALILEO, PCI_FNC_GALILEO, 0x04, 0x0006 |
		pcicfg_read_half(PCI_DEV_GALILEO, PCI_FNC_GALILEO, 0x04));

	pcicfg_write_byte(PCI_DEV_GALILEO, PCI_FNC_GALILEO, 0x0c, DCACHE_LINE_SIZE / 4);

	pcicfg_write_byte(PCI_DEV_GALILEO, PCI_FNC_GALILEO, 0x0d, 64);

	/* set PCI bus timeouts / retries             *
	 *                                            *
	 * apparently early Galileos require the read *
	 * before the write else they lock up. info   *
	 * picked up from kernel source               */

	BRDG_REG_WORD(BRDG_REG_TIMEOUT_RETRY);

	BRDG_REG_WORD(BRDG_REG_TIMEOUT_RETRY) =
		(255 << 16) |		/* retry counter */
		(255 << 8) |		/* timeout #1    */
		255;					/* timeout #0    */

	/* read unit type */

	unit = pcicfg_read_byte(PCI_DEV_VIA, PCI_FNC_VIA_ISA, 0x94) >> 4;

	fixup_count_rate();
}

/*
 * list PCI devices
 */
static void pci_scan(void)
{
	unsigned dev, fnc, id, ss;

	for(dev = 0; dev < PCI_DEVICES_MAX; ++dev)

		for(fnc = 0; fnc < 8; ++fnc) {

			BRDG_REG_WORD(BRDG_REG_INTR_CAUSE) = ~BRDG_INTR_CAUSE_RETRY_CTR;

			id = pcicfg_read_word(dev, fnc, 0);

			if(BRDG_REG_WORD(BRDG_REG_INTR_CAUSE) & BRDG_INTR_CAUSE_RETRY_CTR) {

				printf("%02x.%u ***  bad device  ***\n", dev, fnc);
				break;

			} else if(id != 0xffffffff) {

				ss = pcicfg_read_word(dev, fnc, 0x2c);
				printf("%02x.%u %04x_%04x (%04x_%04x)\n", dev, fnc, id & 0xffff, id >> 16, ss & 0xffff, ss >> 16);
				if(!fnc && !(pcicfg_read_byte(dev, fnc, 0x0e) & 0x80))
					break;

			} else

				break;
		}
}

int cmnd_pci(int opsz)
{
	unsigned dev, fnc, ofs, val;
	char *ptr;

	if(argc == 1) {
		pci_scan();
		return E_NONE;
	}

	if(argc < 3)
		return E_ARGS_UNDER;
	if(argc > 4)
		return E_ARGS_OVER;

	fnc = 0;
	dev = evaluate(argv[1], &ptr);
	if(*ptr == '.')
		fnc = evaluate(ptr + 1, &ptr);
	if(*ptr || dev >= PCI_DEVICES_MAX || fnc > 7) {
		puts("invalid device/function");
		return E_UNSPEC;
	}

	if(!opsz)
		opsz = 4;

	ofs = evaluate(argv[2], &ptr);
	if(*ptr || ofs > 0xff || (ofs & (opsz - 1))) {
		puts("invalid register (must be aligned)");
		return E_UNSPEC;
	}

	if(argc < 4)

		switch(opsz) {
			case 1:
				val = pcicfg_read_byte(dev, fnc, ofs);
				break;
			case 2:
				val = pcicfg_read_half(dev, fnc, ofs);
				break;
			default:
				val = pcicfg_read_word(dev, fnc, ofs);
		}

	else {

		val = evaluate(argv[3], &ptr);
		if(*ptr || (opsz < 4 && (val & (~0 << (opsz * 8)))))
			return E_BAD_VALUE;

		switch(opsz) {
			case 1:
				pcicfg_write_byte(dev, fnc, ofs, val);
				break;
			case 2:
				pcicfg_write_half(dev, fnc, ofs, val);
				break;
			default:
				pcicfg_write_word(dev, fnc, ofs, val);
		}
	}

	printf("%02x.%u %02x = %0*x\n", dev, fnc, ofs, opsz * 2, val);

	return E_NONE;
}

/* vi:set ts=3 sw=3 cin path=include,../include: */
