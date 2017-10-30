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

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "config.h"
#include "wtc.h"
#include "protocols/ecmd/ecmd-base.h"
#include "services/clock/clock.h"

// variables for notifications

e_NotificationStatus_t NotificationStatus=e_NotificationDisabled;
uint8_t NotificationFirstLine[21];
uint8_t NotificationSecondLine[21];
uint8_t NotificationTimestamp[21];

int16_t parse_cmd_wtc_reset(char *cmd, char *output, uint16_t len) {
	wts_init();
	return ECMD_FINAL_OK;
}

int16_t parse_cmd_notification_set(char *cmd, char *output, uint16_t len) {
	uint16_t strLen=strlen(cmd);
	uint16_t ret=ECMD_FINAL_OK;
	clock_datetime_t actTime;

#ifdef WTS_LCD_4LINES
#define Rows	20
#define Lines   2
#else
#define Rows	16
#define Lines   1
#endif


	if(strLen<=1)
	{
		NotificationStatus=e_NotificationDisabled;
		memset(NotificationFirstLine,0,20);
		memset(NotificationSecondLine,0,20);
	} else if(strLen >= Rows)
	{
		// copy first line
    	memcpy(NotificationFirstLine, cmd+1, Rows);
		if (strLen > Lines*Rows){
	    	memcpy(NotificationSecondLine, cmd+1+Rows, Rows);
		} else
		{
	    	memcpy(NotificationSecondLine, cmd+1+Rows, strLen-Rows);
		}
		NotificationStatus=e_NotificationActive;
	} else if (strLen > 1) {
    	memcpy(NotificationFirstLine, cmd+1, strLen-1);
    	NotificationStatus=e_NotificationActive;
    }

	clock_current_localtime(&actTime);

#ifdef WTS_LCD_4LINES
	sprintf((char *)NotificationTimestamp,"%02i:%02i     %02i.%02i.%04i",actTime.hour,actTime.min,actTime.day,actTime.month,actTime.year+1900);
#else
	sprintf((char *)NotificationTimestamp,"%02i:%02i %02i.%02i.%04i",actTime.hour,actTime.min,actTime.day,actTime.month,actTime.year+1900);
#endif

	return ret;
}

/*
-- Ethersex META --
block([[webTempSwitch]])
ecmd_feature(wtc_reset, "wtc_reset",, reset)
ecmd_feature(notification_set, "wtc_not", TEXT, Write TEXT to first line)
*/
