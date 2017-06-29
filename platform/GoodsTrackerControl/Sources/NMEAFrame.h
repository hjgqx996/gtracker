/*
 * NMEAFrame.h
 *
 *  Created on: 28/06/2017
 *      Author: Jefferson
 */

#ifndef SOURCES_NMEAFRAME_H_
#define SOURCES_NMEAFRAME_H_

#include "Array.h"

#define	NMEA_SIZE_MIN_FRAME		30
#define	NMEA_LEN_CHECKSUM		2
#define	NMEA_LEN_TIME			11
#define	NMEA_LEN_DATE			7
#define	NMEA_LEN_ID				6

/*
 * Estrutura de dados do frame NMEA
 *
 */
typedef struct{

	char	Identifier[NMEA_LEN_ID];
	char	Time_UTC[NMEA_LEN_TIME];
	char	Date[NMEA_LEN_DATE];
	float	Lat;
	float	Lng;

	char	LatDirection;
	char	LngDirection;
	char	Status;
	char	SelectionMode;
	char	Mode;
	int		PRNNumber[12];

	int		Speed;
	int		FixQuality;
	int		NumberOfSatelites;
	float	HDOP;
	float	PDOP;
	float	VDOP;
	float	Altitude;
	float	HGeoid;
	float	MagVariation;

} DataNMEA;

#define clearDataNMEA(f) memset((void*)f,0,sizeof(DataNMEA));

#endif /* SOURCES_NMEAFRAME_H_ */
