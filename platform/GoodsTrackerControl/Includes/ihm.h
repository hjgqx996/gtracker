/*
 * ihm.h
 *
 *  Created on: 20/06/2017
 *      Author: Fl�vio Soares
 */
#ifndef SOURCES_IHM_H_
#define SOURCES_IHM_H_

#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

#include "TSS_API.h"

//Eventos do clock
#define	BIT_UPDATE_LCD			0x01
#define	BIT_UPDATE_LCD_XYZ		0x02
#define	BIT_UPDATE_LCD_STAT_COM	0x04
#define	BIT_UPDATE_LCD_STAT_GPS	0x08

typedef enum{

	SCREEN_NONE,
	SCREEN_SPLASH,
	SCREEN_ACCE,
	SCREEN_CLOCK,
	SCREEN_STAT_COM,
	SCREEN_STAT_GPS,

}screen;
/**
 * CHAMANDO ESTA ESTRUTURA DE IHM (INTERFACE HOMEM-M�QUINA) EM VEZ DE
 * MMI (MEN-MACHINE INTERFACE) POR PURO GOSTO PESSOAL... :)
 */

/**
 * Inicializa interface homem máquina
 */
void ihm_init(void);

/**
 *
 */
void ihm_deInit(void);

/**
 * Task paara gerenciamento do ihm
 */
void ihm_task(void);

void initEvents(void);

void printLCD(int linha,int coluna,char* str);
void printClock(void);
void printSplash(void);
void printAccelerometer(void);
void printStatCom(void);
void printStatGPS(void);

void ihm_handle_update(void);
void ihm_event_notify(const EventBits_t uxBitsToSet);

extern TaskHandle_t		xHandleIHMTask;

#endif /* SOURCES_IHM_H_ */
