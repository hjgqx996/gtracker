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
#include <uart_host.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "lcd.h"
#include "gps.h"
#include "Telemetria.h"
#include "uart_gps.h"
#include "clock.h"
#include "ihm.h"

volatile static screen active_screen;
static char line_lcd[25];

/* Tempo do splash em segundos */
static int time_splash;

#if MCUC1_CONFIG_NXP_SDK_2_0_USED
/* Define a estrutura de inicilização para a entrada do botão*/
const gpio_pin_config_t botao_config = {
    kGPIO_DigitalInput, 0,
};
#endif

/* Task IHM*/
static const char*			IHM_TASK_NAME =			"tk_ihm";
#define 					IHM_TASK_PRIORITY		(tskIDLE_PRIORITY+6)
#define						IHM_TASK_STACK_SIZE		(configMINIMAL_STACK_SIZE+100)
static TaskHandle_t			xHandleIHMTask;
static const TickType_t		IHM_TASK_DELAY	= 		(100 / portTICK_PERIOD_MS);

/* Timer */
TimerHandle_t			 	xTimerUpdateStat;
static const char*			UPDATE_TIME_NAME =		"tm_stat";

static void ihm_handle_timer ( TimerHandle_t xTimer );

static portTASK_FUNCTION(run_ihm, pvParameters) {

	while(1) {

		uint32_t uxBits;

		xTaskNotifyWait( 0x0, 0xffffffff , &uxBits, portMAX_DELAY );

//		EventBits_t uxBits	= xEventGroupWaitBits(
//										tlm_events,
//										BIT_UPDATE_LCD,
//										pdTRUE,
//										pdFALSE,
//										portMAX_DELAY );

		if(uxBits & BIT_UPDATE_LCD){

			ihm_update();
		}

		vTaskDelay(IHM_TASK_DELAY);
	}

	vTaskDelete(xHandleIHMTask);
}
//-------------------------------------------------------------------------

void ihm_handle_timer ( TimerHandle_t xTimer ){

	if(time_splash>0){

		// Detecta o momento da transição para o zero
		if(--time_splash<=0){

			ihm_set_active_screen(SCREEN_CLOCK);
		}

	}else{

		ihm_notify_screen_stat();
	}
}
//-------------------------------------------------------------------------

static void createTask(void){

	if (xTaskCreate(
		run_ihm,
		IHM_TASK_NAME,
		IHM_TASK_STACK_SIZE,
		(void*)NULL,
		IHM_TASK_PRIORITY,
		&xHandleIHMTask
	) != pdPASS) {

	}

	xTimerUpdateStat = xTimerCreate (
			UPDATE_TIME_NAME,
			UPDATE_TIME_STAT,
			pdTRUE,
			( void * ) 0,
			ihm_handle_timer
    );

    if( xTimerStart( xTimerUpdateStat, 0 ) != pdPASS ){
    	//Problema no start do timer
    }
}
//--------------------------------------------------------------------------------

void ihm_update(void) {

	//Hook de processamento dos eventos
	switch(active_screen){

		case 	SCREEN_SPLASH:		printSplash();			break;
		case 	SCREEN_CLOCK:		printClock();			break;
		case 	SCREEN_ACCE:		printAccelerometer();	break;
		case 	SCREEN_TANK:		printTank();			break;
		case 	SCREEN_GPS:			printGPS();				break;
		case 	SCREEN_STAT_COM:	printStatCom();			break;
		case 	SCREEN_STAT_GPS:	printStatGPS();			break;
	}
}
//-----------------------------------------------------------------------------------------

void printGPS(void){

	TO_STRING(line_lcd,"Lat:%12.7f",	telemetria.GPS.Lat);
	printLCD(1,1,line_lcd);

	TO_STRING(line_lcd,"Lng:%12.7f",	telemetria.GPS.Lng);
	printLCD(2,1,line_lcd);
}
//-----------------------------------------------------------------------------------------

void printTank(void){

	TO_STRING(line_lcd,"Level: %05d    ",	telemetria.Tank.Level);
	printLCD(1,1,line_lcd);

	TO_STRING(line_lcd,"Lock:  %d         ",	telemetria.Tank.Lock);
	printLCD(2,1,line_lcd);
}
//-----------------------------------------------------------------------------------------

void printAccelerometer(void){

	TO_STRING(line_lcd,"X:%c%4.2f Y:%c%4.2f   ",	telemetria.Accelerometer.x_g>=0?'+':'-',fabs(telemetria.Accelerometer.x_g),
														telemetria.Accelerometer.y_g>=0?'+':'-',fabs(telemetria.Accelerometer.y_g)
													);
	printLCD(1,1,line_lcd);

	TO_STRING(line_lcd,"Z:%c%4.2f         	  ",	telemetria.Accelerometer.z_g>=0?'+':'-',fabs(telemetria.Accelerometer.z_g)
													);
	printLCD(2,1,line_lcd);
}
//-----------------------------------------------------------------------------------------

void printStatCom(void){

	TO_STRING(line_lcd,"RM:%03dP:%03dC:%03d",uart_host_rx_max(),uart_host_rx_head(),uart_host_rx_tail());
	printLCD(1,1,line_lcd);

	TO_STRING(line_lcd,"TM:%03dP:%03dC:%03d",uart_host_tx_max(),uart_host_tx_head(),uart_host_tx_tail());
	printLCD(2,1,line_lcd);
}
//-----------------------------------------------------------------------------------------

void printStatGPS(void){

	TO_STRING(line_lcd,"RX: M:%03d        ",uart_gps_rx_max());
	printLCD(1,1,line_lcd);

	TO_STRING(line_lcd,"RX: C:%03d P:%03d ",uart_gps_rx_tail(),uart_gps_rx_head());
	printLCD(2,1,line_lcd);
}
//-----------------------------------------------------------------------------------------

void printSplash(void){

	printLCD(1,1,"**GOODSTRACKER**");
	printLCD(2,1,"    VER. 0.3    ");
}
//-----------------------------------------------------------------------------------------

void printClock(void){

	rtc_clock	time;

	switch(statuc_clock){

		case CLOCK_INIT:
			TO_STRING(line_lcd,"    CLKINIT    ");

			break;
		case CLOCK_STARTED:
			TO_STRING(line_lcd,"    CLKSTART    ");
			break;
		case CLOCK_UPDATE:
			TO_STRING(line_lcd,"     CLKUPD     ");
			break;
		case CLOCK_ADJUSTED:
			if(getLocalClock(&time)){
#if MCUC1_CONFIG_NXP_SDK_2_0_USED
				TO_STRING(line_lcd," %02d.%02d %02d:%02d:%02d \n",time.day,time.month,time.hour, time.minute, time.second);
#else
				TO_STRING(line_lcd," %02d.%02d %02d:%02d:%02d \n",time.Day,time.Month,time.Hour, time.Minute, time.Second);
#endif
			}else{
				statuc_clock = CLOCK_ERROR;
			}

			break;

		case CLOCK_ERROR:
			TO_STRING(line_lcd,"     ERROR     ");
			break;

	}

	printLCD(1,1,"**GOODSTRACKER**");
	printLCD(2,1,line_lcd);
}
//-----------------------------------------------------------------------------------------

void printLCD(int linha,int col,char* str){

	LCDGotoXY(linha,col);
	LCDWriteString(str);
}
//-----------------------------------------------------------------------------------------

static void ihm_notify_screen(void){

	BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken	= pdFALSE;
    xResult						= pdFAIL;

	xResult = xTaskNotifyFromISR(xHandleIHMTask, BIT_UPDATE_LCD , eSetBits, &xHigherPriorityTaskWoken );

	if (xResult != pdFAIL){

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
//-----------------------------------------------------------------------------------------

void ihm_notify_screen_clock(void){

	if(active_screen == SCREEN_CLOCK){

		ihm_notify_screen();
	}
}
//-----------------------------------------------------------------------------------------

void ihm_notify_screen_stat(void){

	if(active_screen == SCREEN_STAT_COM){

		xTaskNotify(xHandleIHMTask, BIT_UPDATE_LCD , eSetBits);

	}else if(active_screen == SCREEN_STAT_GPS){

		xTaskNotify(xHandleIHMTask, BIT_UPDATE_LCD , eSetBits);
	}
}
//-----------------------------------------------------------------------------------------

void ihm_notify_screen_tlm(void){

	if(active_screen == SCREEN_ACCE){

		xTaskNotify(xHandleIHMTask, BIT_UPDATE_LCD , eSetBits);

	} else if(active_screen == SCREEN_TANK){

		xTaskNotify(xHandleIHMTask, BIT_UPDATE_LCD , eSetBits);

	} else if(active_screen == SCREEN_GPS){

		xTaskNotify(xHandleIHMTask, BIT_UPDATE_LCD , eSetBits);
	}
}
//-----------------------------------------------------------------------------------------

inline void ihm_set_active_screen(screen s){

	if(active_screen != s){

		active_screen = s;

		xTaskNotify(xHandleIHMTask, BIT_UPDATE_LCD , eSetBits);
	}
}
//-----------------------------------------------------------------------------------------

/*
 *
 * Inicializa LCD
 *
 */
void ihm_init(void) {

	active_screen	= -1;		// para grarantir a atualização

	LCDInit();

	time_splash		= TIME_SPLASH;

	// Habilitar o clock dos ports que serão utilizados (PORTD).
	//SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;

	// Seleciona a função de GPIO
	//PORTD_PCR7 = PORT_PCR_MUX(1);

	// Configura como entrada
	//GPIOD_PDDR &= KEY_INPUT;

#if MCUC1_CONFIG_NXP_SDK_2_0_USED
	PORT_SetPinInterruptConfig(KEY_PORT, KEY_GPIO_PIN, kPORT_InterruptRisingEdge);

	NVIC_EnableIRQ(SW_IRQ);

    GPIO_PinInit(KEY_GPIO, KEY_GPIO_PIN, &botao_config);
#endif

	createTask();

	ihm_set_active_screen(SCREEN_SPLASH);
}
//-----------------------------------------------------------------------------------------

void readKey(void){

	screen s = active_screen;

	if(s < (NUM_OF_SCREEN-1)){

		s++;

	}else{

		s = SCREEN_CLOCK;
	}

	active_screen = s;

	ihm_notify_screen();
}
//-----------------------------------------------------------------------------------------

void ihm_deInit(void) {

	//TODO
}
//-----------------------------------------------------------------------------------------

#if MCUC1_CONFIG_NXP_SDK_2_0_USED
/**
 *
 */
void SW_IRQ_HANDLER (void){

	GPIO_ClearPinsInterruptFlags(KEY_GPIO,KEY_GPIO_MASK);

	if(active_screen< (NUM_OF_SCREEN-1)){

		active_screen++;

	}else{

		ihm_set_active_screen(SCREEN_CLOCK);
	}
}
//-----------------------------------------------------------------------------------------
#endif
