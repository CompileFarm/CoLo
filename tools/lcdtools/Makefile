#
# (C) P.Horton 2004,2005,2006
#
# $Id: Makefile 186 2006-01-17 23:03:58Z pdh $
#
# This code is covered by the GNU General Public License. For details see the file "COPYING".
#

SUBDIRS= liblcd putlcd paneld e2fsck-lcd

all:
	set -e; for x in $(SUBDIRS); do $(MAKE) -C $$x; done

clean:
	for x in $(SUBDIRS); do $(MAKE) -C $$x clean; done

.PHONY: all clean
