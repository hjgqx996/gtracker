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

#include "application.h"
#include "clock.h"
#include "ihm.h"

static int time_splah	= 2;		//2 segundos

static ihmStruct ihmGlobal;

TaskHandle_t	xHandleIHMTask;

EventGroupHandle_t	ihm_events;

void ihm_task(void) {

	EventBits_t uxBits	= xEventGroupWaitBits(
									ihm_events,
									BIT_UPDATE_LCD | BIT_UPDATE_LCD_XYZ,
									pdTRUE,
									pdFALSE,
									portMAX_DELAY );


	if(uxBits & BIT_UPDATE_LCD_XYZ){

		if(!time_splah){
			printAccelerometer();
		}
	}

	if(uxBits & BIT_UPDATE_LCD){

		if(time_splah){

			time_splah--;

			// Mostrando o splah

		}else {

			printClock();
		}
	}
}
//-----------------------------------------------------------------------------------------

void printAccelerometer(void){

	char buffer_lcd[17];

	memset(buffer_lcd,0,17);

	XF1_xsprintf(buffer_lcd,"%c%.2f%c%.2f%c%.2f",	telemetria.Accelerometer.x_g>0?'+':'-',fabs(telemetria.Accelerometer.x_g),
													telemetria.Accelerometer.y_g>0?'+':'-',fabs(telemetria.Accelerometer.y_g),
													telemetria.Accelerometer.z_g>0?'+':'-',fabs(telemetria.Accelerometer.z_g)
													);

	printLCD(1,1,buffer_lcd);
}
//-----------------------------------------------------------------------------------------

void printSplah(void){

	printLCD(1,1," .GOODSTRACKER. ");
	printLCD(2,1,"    VER. 0.2    ");
}
//-----------------------------------------------------------------------------------------

void printClock(void){

	LDD_RTC_TTime	time;

	char buffer_lcd[17];
	buffer_lcd[0] = '\0';

	switch(statuc_clock){

		case CLOCK_INIT:
			XF1_xsprintf(buffer_lcd,"    CLKINIT    ");

			break;
		case CLOCK_STARTED:
			XF1_xsprintf(buffer_lcd,"    CLKSTART    ");
			break;
		case CLOCK_UPDATE:
			XF1_xsprintf(buffer_lcd,"     CLKUPD     ");
			break;
		case CLOCK_ADJUSTED:
			if(getLocalClock(&time)){
				XF1_xsprintf(buffer_lcd," %02d.%02d %02d:%02d:%02d \n",time.Day,time.Month,time.Hour, time.Minute, time.Second);

			}else{
				statuc_clock = CLOCK_ERROR;
			}

			break;

		case CLOCK_ERROR:
			XF1_xsprintf(buffer_lcd,"     ERROR     ");
			break;

	}

	printLCD(2,1,buffer_lcd);
}
//-----------------------------------------------------------------------------------------

void printLCD(int linha,int col,char* str){

	LCDGotoXY(linha,col);
	LCDWriteString(str);
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

	printSplah();

	initEvents();
}
//-----------------------------------------------------------------------------------------

void ihm_deInit(void) {

}
//-----------------------------------------------------------------------------------------

void ihm_process_events(ihmStruct *ihm) {

	if (ihm) {
#if 0
		//TODO - ISSO � S� UMA VERS�O PRELIMINAR, O CERTO � TRATAR COMO RingBuffer
		UINT8 evmax = ihm->ihmEventBuffer.head;
		unsigned char i;
		for (i = 0; i < evmax; i++) {
			ihmEvent *ev = &ihm->ihmEventBuffer.event[i];
			ihm_process_event(ev);
		}
#endif
	}
}
//-----------------------------------------------------------------------------------------

static void ihm_process_event(ihmEvent *ev) {

	if (ev) {

		switch (ev->evType) {
		case IHM_EVENT_NONE:
			break;
		case IHM_EVENT_CHOOSE_LEFT:
			LCDClear();
			LCDWriteString("ESQUERDA");
			break;
		case IHM_EVENT_CHOOSE_RIGHT:
			LCDClear();
			LCDWriteString("DIREITA");
			break;
		case IHM_EVENT_CHOOSE_OK:
			LCDClear();
			LCDWriteString("OK");
			break;
		default:
			break;
		}

	}
}
//-----------------------------------------------------------------------------------------

//TODO - IMPLEMENTAR CHAMADA PARA RECEBER EVENTOS EM GERAL, ABSTRAIR MELHOR O HARDWARE
//TODO - POR ENQUANTO FICA ASSIM PARA IMPLEMENTA��O B�SICA
int ihm_put_slide_event(TSS_CSASlider *event) {

	if (event == NULL)
		return -1;

	if (TSSin_cKey0.DynamicStatus.Movement) {
		UINT8 evposit = ihmGlobal.ihmEventBuffer.head;
		if (evposit >= IHM_MAX_EVENTS)
			evposit = 0;
		else
			evposit++;

		ihmGlobal.ihmEventBuffer.head = evposit;

		ihmEvent *ev = &ihmGlobal.ihmEventBuffer.event[evposit];

		if (TSSin_cKey0.Events.Touch) {
			if (!(TSSin_cKey0.Events.InvalidPos)) {

				ev->evType = IHM_EVENT_CHOOSE_OK;
			}
		}

		if (TSSin_cKey0.DynamicStatus.Displacement > (UINT8) 15) {
			if (TSSin_cKey0.DynamicStatus.Direction) {

				ev->evType = IHM_EVENT_CHOOSE_LEFT;
			} else {

				ev->evType = IHM_EVENT_CHOOSE_RIGHT;
			}
		}

	}

	return 0;
}
//-----------------------------------------------------------------------------------------

/*
 * Inicializa a estrutura de eventos
 *
 */
void initEvents(void){

	unsigned char i;
	ihmGlobal.option = 0;

	ihmGlobal.ihmEventBuffer.head = 0;
	ihmGlobal.ihmEventBuffer.tail = 0;

	for (i = 0; i < IHM_MAX_EVENTS; i++) {
		ihmGlobal.ihmEventBuffer.event[i].evType = IHM_EVENT_NONE;
		/*INICIALIZAMOS COMO J� TRATADO PARA N�O ENTRAR A 1a VEZ A TOA*/
		ihmGlobal.ihmEventBuffer.event[i].eventTreated = true;
	}
}
//-----------------------------------------------------------------------------------------
