/*
 * Accelerometer.c
 *
 *  Created on: 29/06/2017
 *      Author: Jefferson
 */

#include "Data.h"
#include "MMA1.h"
#include "AppQueues.h"
#include "Accelerometer.h"

Info	acceInfo;
Info*	pAcceInfo = &acceInfo;

void Accelerometer_Run(void) {

//	MMA1_GetRaw8XYZ(&pAcceInfo->Acc[0]);

	pAcceInfo->Acc[AXIS_X] = MMA1_GetXmg();
	pAcceInfo->Acc[AXIS_Y] = MMA1_GetYmg();
	pAcceInfo->Acc[AXIS_Z] = MMA1_GetZmg();
//	pAcceInfo->AcceXYZ[AXIS_X]	= 20;
//	pAcceInfo->AcceXYZ[AXIS_Y]	= 21;
//	pAcceInfo->AcceXYZ[AXIS_Z]	= 22;

	pAcceInfo->Inc[AXIS_X]	= 30;
	pAcceInfo->Inc[AXIS_Y]	= 31;
	pAcceInfo->Inc[AXIS_Z]	= 32;

    if(xQueueSendToBack( xQueueData , ( void * ) &pAcceInfo, ( TickType_t ) 1 ) ){

    	xTaskNotify( xHandleMainTask , UPDATE_ACCE , eSetBits );
    }
}
//------------------------------------------------------------------------

void initAccel(void){

	if(MMA1_Init()==ERR_OK){

		MMA1_Enable();
	}

	clearInfo(pAcceInfo);
}
//------------------------------------------------------------------------