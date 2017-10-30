/*
 * Copyright (c) 2009 by Stefan Riepenhausen <rhn@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

#ifndef HAVE_WTS_H
#define HAVE_WTS_H

#include <stdio.h>

#define MAXDIGIIN 		4

#define LCD_PAGE_DELAY	1 //must be smaller than 254 see uint8_t lcd_count


#define LCD_ARROW_UP 		0
#define LCD_ARROW_DOWN 		1
#define LCD_ON_OFF 			2

typedef enum e_NotificationStatus {
	e_NotificationDisabled=0,
	e_NotificationActive,
	e_NotificationActiveHold
}e_NotificationStatus_t;

void wts_init();

void wts_lcd_update_data();

void hd44780_print_P (unsigned char line,unsigned char pos, FILE *stream,const char *Buffer,...);

#define hd44780_print(a,b,stream,format, args...)  hd44780_print_P(a,b,stream,PSTR(format) , ## args)

#endif  /* HAVE_WTS_H */
