/*
 * application.h
 *
 *  Created on: 04/06/2017
 *      Author: Jefferson
 */

#ifndef SOURCES_APPLICATION_H_
#define SOURCES_APPLICATION_H_

#include "protocol.h"


typedef enum
	{LED_GREEN	=0,
	LED_RED		=1,
	LED_BLUE	=2}LEDS;


ResultExec onAnalog(DataFrame* cmd);
ResultExec onLED(DataFrame* cmd);
ResultExec onPWM(DataFrame* cmd);
ResultExec onTouch(DataFrame* cmd);
ResultExec onAcel(DataFrame* cmd);

void read_accel();
void read_Channels_AD();
void initCallBacks();
void initAccel();

extern volatile bool AD_finished;

#endif /* SOURCES_APPLICATION_H_ */
