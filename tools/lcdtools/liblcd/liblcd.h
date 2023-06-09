/*
 * (C) P.Horton 2004,2005,2006
 *
 * $Id: liblcd.h 186 2006-01-17 23:03:58Z pdh $
 *
 * This code is covered by the GNU General Public License. For details see the file "COPYING".
 */

#ifndef _PANEL_H_
#define _PANEL_H_

#define BTN_CLEAR					(1 << 1)
#define BTN_LEFT					(1 << 2)
#define BTN_UP						(1 << 3)
#define BTN_DOWN					(1 << 4)
#define BTN_RIGHT					(1 << 5)
#define BTN_ENTER					(1 << 6)
#define BTN_SELECT				(1 << 7)

#define BTN_MASK					(BTN_CLEAR|BTN_LEFT|BTN_UP|BTN_DOWN|BTN_RIGHT|BTN_ENTER|BTN_SELECT)

#define LCD_WIDTH					16

extern const char *getapp(void);

extern int lcd_open(const char *);
extern void lcd_close(void);
extern int lcd_prog(unsigned, const void *);
extern void lcd_puts(unsigned, unsigned, unsigned, const char *);
extern void lcd_clear(void);
extern void lcd_curs_move(unsigned, unsigned);
extern void lcd_text(const char *, unsigned);
extern int btn_read(void);

#ifdef BUILDING_LIBLCD

struct lcd_dispatch_table
{
	void (*close)(void);
	void (*clear)(void);
	void (*curs_move)(unsigned, unsigned);
	void (*prog_char)(unsigned, const void *);
	void (*puts)(unsigned, unsigned, unsigned, const char *);
	void (*text)(const char *, unsigned);
	int (*buttons)(void);
};

extern const struct lcd_dispatch_table *lcddev_open(const char *);
extern const struct lcd_dispatch_table *lcdraw_open(void);

#endif

#endif

/* vi:set ts=3 sw=3 cin: */
