/*
 * Data.h
 *
 *  Created on: 29/06/2017
 *      Author: Jefferson
 */

#ifndef SOURCES_DATATLM_H_
#define SOURCES_DATATLM_H_

#include <stdint.h>
#include <string.h>

typedef struct{

	float	Lat;
	float	Lng;
	int		Speed ;
	char	Time[11];
	char	Date[7];

} DataGPS;

typedef struct{

	int		x,y,z;
	float	x_g,y_g,z_g;

} DataAccelerometer;

typedef struct{

	int		Level;
	int		Lock;

} DataTank;

typedef struct{

	DataGPS				GPS;
	DataTank			Tank;
	DataAccelerometer	Accelerometer;

} DataTLM;

void updateDataLevel(void);
void updateDataAcce(void);
void updateDataGPS(void);
void updateTLM(uint32_t ulNotifiedValue);

#define clearDataTLM(t,f) memset((void*)f,0,sizeof(t));

extern DataTLM			dataTLM;

#endif /* SOURCES_DATATLM_H_ */
