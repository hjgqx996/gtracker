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

typedef enum {
	IHM_EVENT_NONE,
	IHM_EVENT_CHOOSE_LEFT,
	IHM_EVENT_CHOOSE_RIGHT,
	IHM_EVENT_CHOOSE_OK,
} ihmEventType;

typedef struct {
	bool eventTreated;
	ihmEventType evType;
} ihmEvent;

#define IHM_MAX_EVENTS 16
typedef struct {
	uint8_t option;
	struct {
		ihmEvent event[IHM_MAX_EVENTS]; //TODO - USAR BUFFER CIRCULAR
		uint8_t head;
		uint8_t tail;
	} ihmEventBuffer;

} ihmStruct;


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
 *
 */
void ihm_task(void);

/**
 *
 */
int ihm_put_slide_event(TSS_CSASlider *event);

void ihm_process_events(ihmStruct *ihm);

void initEvents(void);

void printLCD(int linha,int coluna,char* str);
void printClock(void);
void printSplah(void);
void printAccelerometer(void);

extern TaskHandle_t		xHandleIHMTask;

extern EventGroupHandle_t	ihm_events;

#endif /* SOURCES_IHM_H_ */
