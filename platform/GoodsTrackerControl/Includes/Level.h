/*
 * Level.h
 *
 *  Created on: 29/06/2017
 *      Author: Jefferson
 */

#ifndef SOURCES_LEVEL_H_
#define SOURCES_LEVEL_H_

#include "AD1.h"

typedef struct{

	int		Level;
	int		Lock;

} Tank;

void runAnalog(void);

extern volatile bool AD_finished;

#endif /* SOURCES_LEVEL_H_ */