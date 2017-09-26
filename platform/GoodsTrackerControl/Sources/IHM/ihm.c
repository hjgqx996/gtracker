/*
 * ihm.c
 *
 *  Created on: 20/06/2017
 *      Author: Fl�vio Soares
 */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "TSSin.h"
#include "lcd.h"
#include "XF1.h"

#include "gps.h"
#include "application.h"
#include "protocol.h"
#include "clock.h"
#include "ihm.h"

static screen active_screen;
static char line_lcd[17];

static int time_splash	= 3;		//3 segundos

TaskHandle_t	xHandleIHMTask;

static EventGroupHandle_t	ihm_events;

void ihm_task(void) {

	EventBits_t uxBits	= xEventGroupWaitBits(
									ihm_events,
									BIT_UPDATE_LCD |
									BIT_UPDATE_LCD_XYZ |
									BIT_UPDATE_LCD_STAT_COM |
									BIT_UPDATE_LCD_STAT_GPS,
									pdTRUE,
									pdFALSE,
									portMAX_DELAY );


	if(uxBits & BIT_UPDATE_LCD_XYZ){

		printAccelerometer();
	}

	if(uxBits & BIT_UPDATE_LCD){

		printClock();
	}

	if(uxBits & BIT_UPDATE_LCD_STAT_COM){

		printStatCom();
	}

	if(uxBits & BIT_UPDATE_LCD_STAT_GPS){

		printStatGPS();
	}
}
//-----------------------------------------------------------------------------------------

void printAccelerometer(void){

	XF1_xsprintf(line_lcd,"%c%.2f%c%.2f%c%.2f",	telemetria.Accelerometer.x_g>0?'+':'-',fabs(telemetria.Accelerometer.x_g),
													telemetria.Accelerometer.y_g>0?'+':'-',fabs(telemetria.Accelerometer.y_g),
													telemetria.Accelerometer.z_g>0?'+':'-',fabs(telemetria.Accelerometer.z_g)
													);

	printLCD(1,1,line_lcd);
}
//-----------------------------------------------------------------------------------------

void printStatCom(void){

	static int max_count_tx=0,max_count_rx=0;

	if(bufferRx.count>max_count_rx){
		max_count_rx = bufferRx.count;
	}

	if(bufferTx.count>max_count_tx){
		max_count_tx = bufferTx.count;
	}

	XF1_xsprintf(line_lcd,"RX:%03d          ",max_count_rx);
	printLCD(1,1,line_lcd);

	XF1_xsprintf(line_lcd,"TX:%03d          ",max_count_tx);
	printLCD(2,1,line_lcd);
}
//-----------------------------------------------------------------------------------------

void printStatGPS(void){

	static int max=0;

	if(bufferRxNMEA.count>max){
		max = bufferRxNMEA.count;
	}

	XF1_xsprintf(line_lcd,"RX:%03d          ",max);
	printLCD(1,1,line_lcd);

	XF1_xsprintf(line_lcd,"RX: C:%03d P:%03d ",bufferRxNMEA.index_consumer,bufferRxNMEA.index_producer);
	printLCD(2,1,line_lcd);
}
//-----------------------------------------------------------------------------------------

void printSplash(void){

	printLCD(1,1,"**GOODSTRACKER**");
	printLCD(2,1,"    VER. 0.3    ");
}
//-----------------------------------------------------------------------------------------

void printClock(void){

	LDD_RTC_TTime	time;

	switch(statuc_clock){

		case CLOCK_INIT:
			XF1_xsprintf(line_lcd,"    CLKINIT    ");

			break;
		case CLOCK_STARTED:
			XF1_xsprintf(line_lcd,"    CLKSTART    ");
			break;
		case CLOCK_UPDATE:
			XF1_xsprintf(line_lcd,"     CLKUPD     ");
			break;
		case CLOCK_ADJUSTED:
			if(getLocalClock(&time)){
				XF1_xsprintf(line_lcd," %02d.%02d %02d:%02d:%02d \n",time.Day,time.Month,time.Hour, time.Minute, time.Second);

			}else{
				statuc_clock = CLOCK_ERROR;
			}

			break;

		case CLOCK_ERROR:
			XF1_xsprintf(line_lcd,"     ERROR     ");
			break;

	}

	printLCD(2,1,line_lcd);
}
//-----------------------------------------------------------------------------------------

void printLCD(int linha,int col,char* str){

	LCDGotoXY(linha,col);
	LCDWriteString(str);
}
//-----------------------------------------------------------------------------------------

void ihm_event_notify(const EventBits_t uxBitsToSet){

	bool flag = false;

	if((active_screen ==SCREEN_CLOCK) && (uxBitsToSet & BIT_UPDATE_LCD)){

		flag = true;
	}

	if((active_screen ==SCREEN_ACCE) && (uxBitsToSet & BIT_UPDATE_LCD_XYZ)){

		flag = true;
	}

	if((active_screen ==SCREEN_STAT_COM) && (uxBitsToSet & BIT_UPDATE_LCD_STAT_COM)){

		flag = true;
	}

	if((active_screen ==SCREEN_STAT_GPS) && (uxBitsToSet & BIT_UPDATE_LCD_STAT_GPS)){

		flag = true;
	}

	if(flag){

		xEventGroupSetBits(ihm_events, uxBitsToSet);
	}
}
//-----------------------------------------------------------------------------------------

void ihm_handle_update(void){

	if(time_splash>0){

		if(--time_splash<=0){

			active_screen = SCREEN_CLOCK;
		}
	}else{

		ihm_event_notify(BIT_UPDATE_LCD);
	}
}
//-----------------------------------------------------------------------------------------


/*
 *
 * Inicializa LCD
 *
 */
void ihm_init(void) {

	ihm_events	= xEventGroupCreate();

	TSSin_Configure(); /* initialize TSS library */

	LCDInit();

	active_screen = SCREEN_SPLASH;

	printSplash();
}
//-----------------------------------------------------------------------------------------

void ihm_deInit(void) {

}
//-----------------------------------------------------------------------------------------
