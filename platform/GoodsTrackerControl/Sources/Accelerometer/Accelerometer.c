/*
 * Accelerometer.c
 *
 *  Created on: 29/06/2017
 *      Author: Jefferson
 */

#include "MMA8451.h"

#include "application.h"
#include "Accelerometer.h"

Accelerometer	acceInfo;

QueueHandle_t	xQueueAcc;

TaskHandle_t xHandleAccelTask;

static const TickType_t xTaskDelay = (200 / portTICK_PERIOD_MS);

void accelerometer_task(void) {

	if(MMA845x_getXYZ(&acceInfo)){

		if(xQueueSendToBack( xQueueAcc ,  &acceInfo, ( TickType_t ) 1 ) ){

			xTaskNotify( xHandleCallBackTask , BIT_UPDATE_ACCE , eSetBits );
		}
	}

	vTaskDelay(xTaskDelay);
}
//------------------------------------------------------------------------

void accelerometer_init(void){

	xQueueAcc		= xQueueCreate( 1, sizeof( Accelerometer ));

	clearAccelerometer(&acceInfo);
	MMA845x_init();
}
//------------------------------------------------------------------------

void deInitAccelerometer(void){

	MMA845x_deInit();
}
//------------------------------------------------------------------------
