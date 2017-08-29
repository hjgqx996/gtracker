/*
 * AppQueues.c
 *
 *  Created on: 29/06/2017
 *      Author: Jefferson
 */

#include <Telemetria.h>
#include "Array.h"
#include "DataFrame.h"
#include "AppQueues.h"

// Tasks Delay
const TickType_t xMainDelay				= (60000 / portTICK_PERIOD_MS);
const TickType_t xCommunicationDelay	= (100 / portTICK_PERIOD_MS);
const TickType_t xDataDelay				= (200 / portTICK_PERIOD_MS);
const TickType_t xIHMDelay				= (100 / portTICK_PERIOD_MS);
const TickType_t xGPSDelay				= (1000 / portTICK_PERIOD_MS);
const TickType_t xAccelDelay			= (100 / portTICK_PERIOD_MS);

// Handles Tasks
TaskHandle_t 	xHandleMainTask		= NULL, xHandleCommunicationTask	= NULL,
				xHandleDataTask		= NULL, xHandleIHMTask				= NULL,
				xHandleGPSTask		= NULL,	xHandleAccelTask			= NULL,
				xHandleCallBackTask	= NULL, xHandleRunTxTask			= NULL;

// Queues
QueueHandle_t	xQueueCom, xQueueLCD, xQueueAnswer, xQueueGPS, xQueueAcc , xQueueTank;

/**
 * Cria todas as filas de menssagens
 *
 */
void initQueues(void){

	xQueueCom		= xQueueCreate( 5, sizeof( DataCom* ));
	xQueueAnswer	= xQueueCreate( 5, sizeof( ArrayPayLoad* ));
	xQueueGPS		= xQueueCreate( 3, sizeof( DataGPS* ));
	xQueueAcc		= xQueueCreate( 2, sizeof( DataAccelerometer* ));
	xQueueTank		= xQueueCreate( 2, sizeof( DataTank* ));
}
//-----------------------------------------------------------------------------
