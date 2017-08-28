/*
 * clock.h
 *
 *  Created on: Aug 28, 2017
 *      Author: Jefferson
 */

#ifndef INCLUDES_CLOCK_H_
#define INCLUDES_CLOCK_H_

#include <stdint.h>
#include "RTC1.h"

#define	FUSO_HORARIO_BR		-3

#define	LEN_TIME_STAMP		12
#define SEC_PER_MIN         60
#define SEC_PER_HOUR        3600
#define SEC_PER_DAY         86400
#define MOS_PER_YEAR        12
#define EPOCH_YEAR          1970
#define IS_LEAP_YEAR(year)  ( (((year)%4 == 0) && ((year)%100 != 0)) || ((year)%400 == 0) )

static int days_per_month[2][MOS_PER_YEAR] = {
  { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
  { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static int days_per_year[2] = {
  365, 366
};

uint32_t getCurrentTimeStamp();
void initClock();
void setClock(LDD_RTC_TTime* time);
void getClock();
void updateTimeStamp();
void setClockByString(char* time,char* date);

uint32_t unix_time_in_seconds(uint8_t sec, uint8_t min, uint8_t hrs,
		uint8_t day, uint8_t mon, uint16_t year);


typedef struct {

	char		str[LEN_TIME_STAMP];
	char		strTime [8];
	uint32_t	val;

}timestamp_t;

extern timestamp_t timestamp;

#endif /* INCLUDES_CLOCK_H_ */
