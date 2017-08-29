/*
 * clock.c
 *
 *  Created on: Aug 28, 2017
 *      Author: Jefferson
 */
#include "Cpu.h"
#include "stdlib.h"
#include "string.h"
#include "XF1.h"
#include "clock.h"

// Referencias:	https://github.com/msolters/make-unix-timestamp-c
//				https://community.nxp.com/docs/DOC-94734

char *DayOfWeekName[] = {
  "Dom",
  "Seg",
  "Ter",
  "Qua",
  "Qui",
  "Sex",
  "Sab"
};

/* User includes (#include below this line is not maintained by Processor Expert) */
LDD_TDeviceData		*MyRTCPtr;
LDD_RTC_TTime		Time;
LDD_TError			Error;

void initClock(){

	MyRTCPtr = RTC1_Init((LDD_TUserData *)NULL, FALSE);        /* Initialize the device, preserve time settings */
}
//-------------------------------------------------------------------------------------------------------------------

/*
 *
 * Configura novo horario
 *
 *
 */
void setClockByString(char* time,char* date){

	LDD_RTC_TTime date_time;

	char year[5];	year[4]		= '\0';
	char month[3];	month[2]	= '\0';
	char day[3];	day[2]		= '\0';
	char hrs[3];	hrs[2] 		= '\0';
	char min[3];	min[2] 		= '\0';
	char sec[3];	sec[2] 		= '\0';

	strncpy(hrs, 	time,2);
	strncpy(min, 	time+2,2);
	strncpy(sec, 	time+4,2);
	strncpy(day, 	date,2);
	strncpy(month,	date+2,2);
	strncpy(year,	"20",2);	// ano 4 digitos
	strncpy(year+2,	date+4,2);

	date_time.Hour		= atoi(hrs);
	date_time.Minute	= atoi(min);
	date_time.Second	= atoi(sec);
	date_time.Year		= atoi(year);
	date_time.Month		= atoi(month);
	date_time.Day		= atoi(day);

	setClock(&date_time);
}
//-------------------------------------------------------------------------------------------------------------------

void setClock(LDD_RTC_TTime* time){

	Time	= *time;
	Error	= RTC1_SetTime(MyRTCPtr, &Time);

	if(Error==ERR_OK){
		RTC1_GetTime(MyRTCPtr, &Time);
	}

	Time.timestamp = getCurrentTimeStamp();
}
//-------------------------------------------------------------------------------------------------------------------

void getClock(){

	RTC1_GetTime(MyRTCPtr, &Time);
	//XF1_xsprintf(timestamp,"Current time: %d:%d:%d %d.%d.%d %s\n", Time.Hour, Time.Minute, Time.Second, Time.Day, Time.Month, Time.Year, DayOfWeekName[Time.DayOfWeek]);
}
//-------------------------------------------------------------------------------------------------------------------

void updateEntityClock(){

	RTC1_GetTime(MyRTCPtr, &Time);
	Time.timestamp++;
}
//-------------------------------------------------------------------------------------------------------------------

uint32_t getCurrentTimeStamp(){

	return unix_time_in_seconds((uint8_t)Time.Second, (uint8_t)Time.Minute, (uint8_t)Time.Hour,
			(uint8_t)Time.Day, (uint8_t)Time.Month, (uint16_t)Time.Year);
}
//-------------------------------------------------------------------------------------------------------------------

uint32_t unix_time_in_seconds(uint8_t sec, uint8_t min, uint8_t hrs,
		uint8_t day, uint8_t mon, uint16_t year) {

	uint32_t ts = 0;

	//  Add up the seconds from all prev years, up until this year.
	uint8_t years = 0;
	uint8_t leap_years = 0;
	for (uint16_t y_k = EPOCH_YEAR; y_k < year; y_k++) {
		if (IS_LEAP_YEAR(y_k)) {
			leap_years++;
		} else {
			years++;
		}
	}
	ts += ((years * days_per_year[0]) + (leap_years * days_per_year[1]))
			* SEC_PER_DAY;

	//  Add up the seconds from all prev days this year, up until today.
	uint8_t year_index = (IS_LEAP_YEAR(year)) ? 1 : 0;
	for (uint8_t mo_k = 0; mo_k < (mon - 1); mo_k++) { //  days from previous months this year
		ts += days_per_month[year_index][mo_k] * SEC_PER_DAY;
	}
	ts += (day - 1) * SEC_PER_DAY; // days from this month

	//  Calculate seconds elapsed just today.
	ts += hrs * SEC_PER_HOUR;
	ts += min * SEC_PER_MIN;
	ts += sec;

	return ts;
}
//---------------------------------------------------------------------------------------------------------
