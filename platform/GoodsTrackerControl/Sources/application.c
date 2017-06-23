/*
 * application.c
 *
 *  Created on: 04/06/2017
 *      Author: Jefferson
 */

#include <stdio.h>
#include "LED_B.h"
#include "LED_G.h"
#include "LED_R.h"
#include "AD1.h"
#include "MMA1.h"
#include "application.h"

volatile	bool AD_finished=FALSE;
uint16_t	AD_Values[AD1_CHANNEL_COUNT];
uint8_t		xyz[3];

void initCallBacks(){

	setEventCMD(CMD_LED,	onLED);
	setEventCMD(CMD_ANALOG,	onAnalog);
	setEventCMD(CMD_ACC,	onAcel);
	setEventCMD(CMD_TOUCH,	onTouch);
	setEventCMD(CMD_PWM,	onPWM);
}
//-------------------------------------------------------------------------

ResultExec onLED(DataFrame* frame){

	return EXEC_SUCCESS;
}
	//-------------------------------------------------------------------------

ResultExec onAnalog(DataFrame* frame){

	return EXEC_SUCCESS;
}
//-------------------------------------------------------------------------

ResultExec onAcel(DataFrame* frame){

	buildPayLoad(frame);

	return EXEC_SUCCESS;
}
//-------------------------------------------------------------------------

ResultExec onTouch(DataFrame* frame){

	return EXEC_SUCCESS;
}
//-------------------------------------------------------------------------

ResultExec onPWM(DataFrame* frame){

	return EXEC_SUCCESS;
}
//------------------------------------------------------------------------

void read_Channels_AD(){

//	if(AD_finished){
//		if(AD1_GetValue16(AD_Values)==ERR_OK){
			//TODO
//		}
//	}

#if 0
	if(AD1_Measure(FALSE)==ERR_OK){

		AD_finished = FALSE;

		while (!AD_finished) {}

		if(AD1_GetValue16(AD_Values)==ERR_OK){
			//TODO
		}
	}
#endif
}
//------------------------------------------------------------------------

void read_accel() {

	MMA1_GetRaw8XYZ(xyz);
}
//------------------------------------------------------------------------

void initAccel(){

	MMA1_Init();

	MMA1_Enable();
}
//------------------------------------------------------------------------

void buildPayLoad(DataFrame* frame){

	char p[LEN_MAX_PAYLOAD];

	sprintf(p,"%s%d:%d:%d:%d",frame->resource,frame->address,xyz[0],xyz[1],xyz[2]);

	setPayLoad(frame,p);
}
//------------------------------------------------------------------------

