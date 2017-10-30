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

#include "wtc.h"

#include "config.h"
#ifdef DEBUG_APP_SAMPLE
# include "core/debug.h"
# define APPSAMPLEDEBUG(a...)  debug_printf("app sample: " a)
#else
# define APPSAMPLEDEBUG(a...)
#endif


#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include <ctype.h> //isdigit ()

#include "hardware/onewire/onewire.h"
#include "hardware/lcd/hd44780.h"
#include "protocols/uip/uip.h" // for mac adress
#include "core/portio/portio.h"
#include "core/portio/named_pin.h"
#include "core/portio/user_config.h"
#include "core/bit-macros.h"
#include "services/clock/clock.h"
#include "services/cron/cron.h"
#include "config.h"
#include "core/debug.h"
#include "hardware/dht/dht.h"
#include "core/util/fixedpoint.h"


#ifndef NELEMS
#define NELEMS(x) (sizeof(x)/sizeof(x[0]))
#endif

#define MAX_DHT 5


//typdefs
typedef struct {
	int16_t min;
	int16_t max;
} range_t;

//local valriables
uint8_t count=0;
uint8_t lineInput[MAXDIGIIN]={2,2,3,3};
uint8_t rowInput[MAXDIGIIN]={0,10,0,10};
range_t range_ow[OW_SENSORS_COUNT];
range_t range_dht_temp[MAX_DHT];
range_t range_dht_humid[MAX_DHT];
uint8_t lcd_count=0,lcdPage=0;

//struct fat_file_struct* fd; // file diskriptor on SD-Card

// variables for notifications
extern e_NotificationStatus_t NotificationStatus;
extern uint8_t NotificationFirstLine[];
extern uint8_t NotificationSecondLine[];
extern uint8_t NotificationTimestamp[];

#define LCD_PER_CENT 	0x25
#define LCD_DEGREE 		0xdf

// local funcntion prototpyes
void wts_reset_peak_values ();


void wts_init()
{
	wts_reset_peak_values();

	// Logging Init
	//cron_jobinsert_callback(-5,-1,-1,-1,-1,INFINIT_RUNNING, CRON_APPEND,log_data,0,NULL);

//	uint8_t lcdArrowUp[8]={0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x04, 0x00};
//	uint8_t lcdArrowDown[8]={0x04, 0x04, 0x04, 0x04, 0x1F, 0x0E, 0x04, 0x00};
//	uint8_t lcdOnOFF[8]={0x1F, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1F};
//	hd44780_define_char(LCD_ARROW_UP,lcdArrowDown,1);
//	hd44780_define_char(LCD_ARROW_DOWN,lcdArrowUp,1);
//	hd44780_define_char(LCD_ON_OFF,lcdOnOFF,1);

	//display Init
	hd44780_clear();
#if WTS_LCD_4LINES
	hd44780_print(0,0,&lcd,"WTC Rev ");
	hd44780_print(0,9,&lcd,GIT_VERSION);
//	hd44780_print(1,0,&lcd, VERSION_STRING_FROM);
	hd44780_print(2,0,&lcd,CONF_ENC_IP);
	hd44780_print(3,0,&lcd,"%02x:%02x:%02x:%02x:%02x:%02x\n",
			uip_ethaddr.addr[0],
			uip_ethaddr.addr[1],
			uip_ethaddr.addr[2],
			uip_ethaddr.addr[3],
			uip_ethaddr.addr[4],
			uip_ethaddr.addr[5]
			);
#else
	hd44780_print(0,0,&lcd,"WTC  Rev. ");
	hd44780_print(0,10,&lcd,GIT_VERSION);
	hd44780_print(1,0,&lcd,CONF_ENC_IP);
#endif

}

// removed because is not neccessary
//void lcd_upactTime_time()
//{
//	struct clock_actTimetime_t actTime;
//	clock_current_localtime(&actTime);
//	//TODO : maybe split off actTime calculation to save time for the loop
//#if WTS_LCD_4LINES
//	hd44780_print(0,0,&lcd,"%02i:%02i %02i.%02i.%02i",actTime.hour,actTime.min, actTime.day,actTime.month,actTime.year%100);
//#endif
//}

char readNamedPin(uint8_t pincfg) {
	uint8_t port = IO_PORTS, pin = 8;
	uint8_t active_high = 1;

	uint8_t val = 2;
	char retVal = 'E';

	if (pincfg < NAMEDPIN_CNT) {
		port = pgm_read_byte(&portio_pincfg[pincfg].port);
		pin = pgm_read_byte(&portio_pincfg[pincfg].pin);
		active_high = pgm_read_byte(&portio_pincfg[pincfg].active_high);

		if (port < IO_PORTS && pin < 8) {

			val = XOR_LOG(vport[port].read_pin (port) & _BV (pin),
					!(active_high));
			if (val == 1) {
				retVal = '*';
			} else if (val == 0) {
				retVal = '-';
			} else // error
			{
				retVal = 'e';
			}
		} else {
			retVal = 'f';

		}
	} else {
		retVal = 'F';
		hd44780_print(0,0,&lcd,"ERROR %i>%i.",pincfg,NAMEDPIN_CNT);

	}

	return retVal;
}

#if WTS_LCD_4LINES
void printValue(uint8_t rowOffset, int16_t value, int16_t min, int16_t max)
{
	int8_t valDigit=abs(value%10);
	int8_t minDigit=abs(max%10);
	int8_t maxDigit=abs(max%10);
	value/=10;
	min/=10;
	max/=10;

	hd44780_print(0+rowOffset,14,&lcd, "%3i.%1i" ,value, valDigit);
	hd44780_print(1+rowOffset,0,&lcd,"Min%3i.%1i   Max%3i.%1i" ,min, minDigit, max, maxDigit);
}
#else
void printValue(uint8_t rowOffset, int16_t value, int16_t min, int16_t max)
{
	int8_t valDigit=abs(value%10);
	int8_t minDigit=abs(max%10);
	int8_t maxDigit=abs(max%10);
	value/=10;
	min/=10;
	max/=10;

	hd44780_print(0+rowOffset,10,&lcd, "%3i.%1i" ,value, valDigit);
	hd44780_print(1+rowOffset,0,&lcd,"Mi%3i.%1i  Ma%3i.%1i" ,min, minDigit, max, maxDigit);
}
#endif

void wts_store_peak_values(uint8_t ow_sensor_cnt)
{
	for(uint8_t idx=0; idx<dht_sensors_count; idx++)
	{
		if(dht_sensors[idx].temp < range_dht_temp[idx].min)
			range_dht_temp[idx].min = dht_sensors[idx].temp;
		if(dht_sensors[idx].temp > range_dht_temp[idx].max)
			range_dht_temp[idx].max = dht_sensors[idx].temp;
		if(dht_sensors[idx].humid < range_dht_humid[idx].min)
			range_dht_humid[idx].min = dht_sensors[idx].humid;
		if(dht_sensors[idx].humid > range_dht_humid[idx].max)
			range_dht_humid[idx].max = dht_sensors[idx].humid;
	}
	for(uint8_t idx=0; idx<ow_sensor_cnt; idx++)
	{
		if(ow_sensors[idx].temp.val < range_ow[idx].min)
			range_ow[idx].min = ow_sensors[idx].temp.val;
		if(ow_sensors[idx].temp.val > range_ow[idx].max)
			range_ow[idx].max = ow_sensors[idx].temp.val;
	}
}
void wts_reset_peak_values ()
{
	for(uint8_t idx=0; idx<dht_sensors_count; idx++)
	{
		range_dht_temp[idx].min = dht_sensors[idx].temp;
		range_dht_temp[idx].max = dht_sensors[idx].temp;
		range_dht_humid[idx].min = dht_sensors[idx].humid;
		range_dht_humid[idx].max = dht_sensors[idx].humid;
	}
	for (uint8_t idx = 0; idx < OW_SENSORS_COUNT; idx++)
	{
		if(ow_sensors[idx].present)
		{
			range_ow[idx].min = ow_sensors[idx].temp.val;
			range_ow[idx].max = ow_sensors[idx].temp.val;
		}
	}
}

unsigned char upactTime_busy;

void hd44780_print_P (unsigned char line,unsigned char pos, FILE *stream,const char *Buffer,...)
{
	va_list ap;
	va_start (ap, Buffer);

	int format_flag;
	char str_buffer[10];
	char str_null_buffer[10];
	char moveBuffer = ' ';
	char move = 0;
	char Base = 0;
	int tmp = 0;
	char strLenght = 0;
	char by;
	char *ptr;

	hd44780_goto(line,pos);

	//Ausgabe der Zeichen
    for(;;)
	{
		by = pgm_read_byte(Buffer++);
		if(by==0) break; // end of format string

		if (by == '%')
		{
            by = pgm_read_byte(Buffer++);
			if (isdigit(by)>0 && by=='0')
				{
				moveBuffer = '0';
                by = pgm_read_byte(Buffer++);
				}
			if (isdigit(by)>0)
				{

 				str_null_buffer[0] = by;
				str_null_buffer[1] = '\0';
				move = atoi(str_null_buffer);
				strLenght=0;
                by = pgm_read_byte(Buffer++);
				}
			else
				{
				move=0;
				}
			Base=0;
			switch (by)
				{
				case 's':
                    ptr = va_arg(ap,char *);
                    strLenght=strlen(ptr);
                    // deal with to short strings filling up blanks at the beginning
                    while(strLenght < move)
                    {
                    	hd44780_putc(' ',stream);
                    	strLenght++;
                    }
                    strLenght=0;
                    // print up to defined length
                    while(*ptr ) {
                    	if(move != 0 && strLenght++ >= move)
                    		break;

                    	hd44780_putc(*ptr++,stream);
                    }
                    break;
				case 'c':
					//Int to char
					format_flag = va_arg(ap,int);
					hd44780_putc(format_flag++,stream);
					break;
				case 'b':
					if(Base==0)
						Base = 2;
				case 'i':
					if(Base==0)
						Base = 10;
				case 'o':
					if(Base==0)
						Base = 8;
				case 'x':
					if(Base==0)
						Base = 16;
				default:
					if(Base==0)
						break;
					itoa(va_arg(ap,int),str_buffer,Base);
					int b=0;
					while (str_buffer[b++] != 0){};
					b--;
					if (b<move)
						{
						move -=b;
						for (tmp = 0;tmp<move;tmp++)
							{
							str_null_buffer[tmp] = moveBuffer;
							}
						//tmp ++;
						str_null_buffer[tmp] = '\0';
						strcat(str_null_buffer,str_buffer);
						strcpy(str_buffer,str_null_buffer);
						}
					hd44780_puts(str_buffer,stream);
					move =0;
					break;
				}

			}
		else
			{
			hd44780_putc(by,stream);
			}
		}
	va_end(ap);
}

void wts_lcd_update_data()
{
//	int16_t temp,temp2;
	uint8_t diBuff=0;
	clock_datetime_t actTime;
	int foundSensors=0;
	int idx=0;
	char value[7];
	char inpVal;
#ifndef DHT_SUPPORT
	uint8_t dht_sensors_count=0;
#endif

	if(upactTime_busy)  //skip multiple execution of upactTime
		return;
	else
		upactTime_busy=1;

  /* prepare existing sensors */
  for (uint8_t i = 0; i < OW_SENSORS_COUNT; i++)
	if(ow_sensors[i].present)
		foundSensors++;

  /* handle min/max values */

  wts_store_peak_values(foundSensors);

	//----------------lcd output----------------------

	clock_current_localtime(&actTime);
	if(NotificationStatus==e_NotificationActive)
	{
		hd44780_clear();

#ifdef WTS_LCD_4LINES
		hd44780_print(0,0,&lcd,"%s",NotificationTimestamp);
		hd44780_print(2,0,&lcd,"%s",NotificationFirstLine);
		hd44780_print(3,0,&lcd,"%s",NotificationSecondLine);
#else
		hd44780_print(0,0,&lcd,"%s",NotificationFirstLine);
		hd44780_print(1,0,&lcd,"%s",NotificationSecondLine);
#endif
		NotificationStatus=e_NotificationActiveHold;
	}
	else if(NotificationStatus==e_NotificationActiveHold)
	{
		// notification is still on dislplay, so no need to refresh.
	}else
	{

#if WTS_LCD_4LINES

#ifdef DHT_SUPPORT
		if(lcdPage < (dht_sensors_count))
				{
				hd44780_clear();

				idx=lcdPage;

				  uint16_t len=7;
				  snprintf_P(&value[0], len, PSTR("%S"), dht_sensors[idx].name);

						if(strcmp(value,""))
						{
							hd44780_print(0,0,&lcd,"%s",value);
							hd44780_print(2,0,&lcd,"%s",value);
						}
						else
						{
							hd44780_print(0,0,&lcd,"DHT %i",idx);
							hd44780_print(2,0,&lcd,"DHT %i",idx);
						}

						printValue(0,dht_sensors[idx].temp, range_dht_temp[idx].min,range_dht_temp[idx].max);
						printValue(2,dht_sensors[idx].humid, range_dht_humid[idx].min,range_dht_humid[idx].max);

						// add special characters in the end.
						hd44780_goto(0,19);
						hd44780_putc(LCD_DEGREE,&lcd);
						hd44780_goto(1,8);
						hd44780_putc(LCD_DEGREE,&lcd);
						hd44780_goto(1,19);
						hd44780_putc(LCD_DEGREE,&lcd);
						hd44780_goto(2,19);
						hd44780_putc(LCD_PER_CENT,&lcd);
						hd44780_goto(3,8);
						hd44780_putc(LCD_PER_CENT,&lcd);
						hd44780_goto(3,19);
						hd44780_putc(LCD_PER_CENT,&lcd);
					}
					else
#endif
						if(lcdPage < (foundSensors + dht_sensors_count)) //one page for each sensor
					{
						idx=lcdPage-dht_sensors_count;

						hd44780_clear();
						if(strcmp(ow_sensors[idx].name,""))
							hd44780_print(0,0,&lcd,"%s",ow_sensors[idx].name);
						else
							hd44780_print(0,0,&lcd,"Sensor_%i",idx);

						if(ow_sensors[idx].temp.twodigits)
						{
							printValue(0, ow_sensors[idx].temp.val/10, range_ow[idx].min/10, range_ow[idx].max/10);
						}
						else
						{
							printValue(0, ow_sensors[idx].temp.val, range_ow[idx].min, range_ow[idx].max);
						}
						// add special characters in the end.
						hd44780_goto(0,19);
						hd44780_putc(LCD_DEGREE,&lcd);
						hd44780_goto(1,8);
						hd44780_putc(LCD_DEGREE,&lcd);
						hd44780_goto(1,19);
						hd44780_putc(LCD_DEGREE,&lcd);
					}
					else  //info page and digital outputs
					{
						hd44780_clear();
						hd44780_print(0,0,&lcd,"%02i:%02i",actTime.hour,actTime.min);
						hd44780_print(0,10,&lcd,"%02i.%02i.%04i",actTime.day,actTime.month,actTime.year+1900);
						diBuff=vport[2].read_port(2);
						for(int i=0;i<8;i++)
						{
							if(((diBuff>>i)&0x1))
								hd44780_print(1,i*2+2,&lcd,"*");
							else
								hd44780_print(1,i*2+2,&lcd,"-");
						}
						// display named input
						uint8_t inCount=0;
						for (uint8_t i = 0; i<MAXDIGIIN && i < NAMEDPIN_CNT; i++)
						  {
							if( pgm_read_byte (&portio_pincfg[i].input)) /*print only inputs */
							{
								/* get named-pin from array */
							  const char * text = (const char *) pgm_read_word (&portio_pincfg[i].name);
							  uint8_t lineLength = strlen_P (text);
							  memcpy_P (value, text, lineLength);

							  inpVal=readNamedPin(i);
						    hd44780_print(lineInput[inCount],rowInput[inCount],&lcd,"%c %7s",inpVal, value);
						    inCount++;
							}
						  }
					}

				if(lcdPage >= (foundSensors + dht_sensors_count*2)) //one info page (foundSensors) and one for each sensor (0..foundSensors-1)
					lcdPage=0;
				else
					lcdPage++;

#else

#ifdef DHT_SUPPORT
		if(lcdPage < (dht_sensors_count*2))
		{
			idx=lcdPage/2;

			hd44780_clear();
			uint16_t len=7;
			snprintf_P(&value[0], len, PSTR("%S"), dht_sensors[idx].name);

			if(strcmp(value,""))
			{
				hd44780_print(0,0,&lcd,"%s",value);
			}
			else
			{
				hd44780_print(0,0,&lcd,"DHT %i",idx);
			}

			if(lcdPage%2 == 0) //temperature
			{
				printValue(0,dht_sensors[idx].temp, dht_sensors[idx].minTemp,dht_sensors[idx].maxTemp);
				hd44780_goto(0,15);
				hd44780_putc(LCD_DEGREE,&lcd);
			}
			else // Humidity
			{
				printValue(0,dht_sensors[idx].humid, dht_sensors[idx].minHumid,dht_sensors[idx].maxHumid);
				// add special characters in the end.
				hd44780_goto(0,15);
				hd44780_putc(LCD_PER_CENT,&lcd);
			}
		}
		else
#endif
			if(lcdPage < (foundSensors + dht_sensors_count*2)) //one page for each sensor
		{
			idx=lcdPage-dht_sensors_count*2;

			hd44780_clear();
			if(strcmp(ow_sensors[idx].name,""))
				hd44780_print(0,0,&lcd,"%s",ow_sensors[idx].name);
			else
				hd44780_print(0,0,&lcd,"Sensor_%i",idx);

			if(ow_sensors[idx].temp.twodigits)
			{
				printValue(0, ow_sensors[idx].temp.val/10, ow_sensors[idx].minTemp.val/10, ow_sensors[idx].maxTemp.val/10);
			}
			else
			{
				printValue(0, ow_sensors[idx].temp.val, ow_sensors[idx].minTemp.val, ow_sensors[idx].maxTemp.val);
			}
			// add special characters in the end.
			hd44780_goto(0,15);
			hd44780_putc(LCD_DEGREE,&lcd);
		}
		else if(lcdPage == (foundSensors + dht_sensors_count*2))
		{
			// display named input
			uint8_t inCount=0;
			for (uint8_t i = 0; i<MAXDIGIIN && i < NAMEDPIN_CNT; i++)
			  {
				if( pgm_read_byte (&portio_pincfg[i].input)) /*print only inputs */
				{
					/* get named-pin from array */
				  const char * text = (const char *) pgm_read_word (&portio_pincfg[i].name);
				  uint8_t lineLength = strlen_P (text);
				  memcpy_P (value, text, lineLength);

				  inpVal=readNamedPin(i);
				  hd44780_print(lineInput[inCount]-2,rowInput[inCount] == 10 ? 9 : 0,&lcd,"%c %5s",inpVal, value);
				  inCount++;
				}
			  }
		}else //info page and digital outputs
		{
			hd44780_clear();
			hd44780_print(0,0,&lcd,"%02i:%02i",actTime.hour,actTime.min);
			hd44780_print(0,6,&lcd,"%02i.%02i.%04i",actTime.day,actTime.month,actTime.year+1900);
			diBuff=vport[2].read_port(2);
			for(int i=0;i<8;i++)
			{
				if(((diBuff>>i)&0x1))
					hd44780_print(1,i*+2,&lcd,"*");
				else
					hd44780_print(1,i*+2,&lcd,"-");
			}
		}


	if(lcdPage >= (foundSensors + dht_sensors_count*2 + 1)) //one info page (foundSensors) and one for each sensor (0..foundSensors-1)
		lcdPage=0;
	else
		lcdPage++;

	#endif

}
	upactTime_busy=0;
}

/*
  -- Ethersex META --
  header(services/webTempClient/wtc.h)
  startup(wts_init)
  timer(250,wts_lcd_update_data())
*/
