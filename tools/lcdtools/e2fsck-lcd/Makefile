#
# (C) P.Horton 2004,2005,2006
#
# $Id: Makefile 186 2006-01-17 23:03:58Z pdh $
#
# This code is covered by the GNU General Public License. For details see the file "COPYING".
#

TARG= e2fsck-lcd
OBJS= e2fsck-lcd.o

include ../../../Rules.mak

CFLAGS= -Werror -Wall -Wstrict-prototypes -fomit-frame-pointer -O2 -pipe -fno-strict-aliasing $(CFLAGS_CPU)
CPPFLAGS= -I../liblcd
LOADLIBES= -L../liblcd -llcd

$(TARG): $(OBJS)

clean:
	rm -f $(TARG) $(OBJS)

.PHONY: clean
