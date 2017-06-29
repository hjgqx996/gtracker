/*
 *
 *  Created on: 27/06/2017
 *      Author: Jefferson
 */

#include "AppQueues.h"
#include "queue.h"
#include "RingBuffer.h"
#include "gps.h"

const char* GGA	= "GGA";
const char* RMC	= "RMC";
const char* GSA	= "GSA";

#define		SOUTH	'S'
#define		WEST	'W'

RingBuffer	bufferRxNMEA;
ArrayFrame	frameNMEA;
StatusNMEA	statusNMEA = NMEA_INIT;
DataNMEA	dataNMEA;
DataNMEA*	pDataNMEA = &dataNMEA;

void NMEA_process(void) {

	switch(statusNMEA){

		default:
		case NMEA_INIT:			NMEA_init();			break;
		case NMEA_INIT_OK:		NMEA_rxStart();			break;
		case NMEA_RX_START:		NMEA_receiveFrame();	break;
		case NMEA_RX_FRAME:		NMEA_receiveFrame();	break;
		case NMEA_RX_END:		NMEA_receiveCheckSum();	break;
		case NMEA_RX_CHECKSUM:	NMEA_rxCR();			break;
		case NMEA_RX_CR:		NMEA_rxLF();			break;
		case NMEA_RX_LF:		NMEA_verifyFrame();		break;
		case NMEA_FRAME_OK:		NMEA_acceptRxFrame();	break;
		case NMEA_FRAME_NOK:	NMEA_errorRxFrame();	break;
		case NMEA_EXEC:			NMEA_sendResult();		break;
		case NMEA_EXEC_ERROR:	NMEA_errorExec();		break;
	}
}
//------------------------------------------------------------------------

static void NMEA_rxStart(void)
{
	char ch;

	if(getGPSData(&ch)){

		if(ch==NMEA_CHAR_START){

			clearArrayFrame(&frameNMEA);

			setGPSStatus(NMEA_RX_START);
		}
	}
}
//------------------------------------------------------------------------

static void NMEA_receiveFrame(void)
{
	char ch;

	if(getGPSData(&ch)) {

		if(ch==NMEA_CHAR_START || frameNMEA.Count>=SIZE_FRAME) {

			setGPSStatus(NMEA_FRAME_NOK);
		}
		else
		  if(ch==NMEA_CHAR_END) {

			 if(frameNMEA.Count>=NMEA_SIZE_MIN_FRAME) {

				 setGPSStatus(NMEA_RX_END);
			 }
			 else {

				 setGPSStatus(NMEA_FRAME_NOK);
			 }
		}
		else {

			putDataArray(&frameNMEA,ch);
			setGPSStatus(NMEA_RX_FRAME);
		}
	}
}
//------------------------------------------------------------------------

static void NMEA_receiveCheckSum(){

	char ch;

	if(getGPSData(&ch)){

		frameNMEA.checksum[(frameNMEA.CountCheckSum++)%NMEA_LEN_CHECKSUM] = ch;

		if(frameNMEA.CountCheckSum >= NMEA_LEN_CHECKSUM){

			frameNMEA.checksum[NMEA_LEN_CHECKSUM] = 0;

			setGPSStatus(NMEA_RX_CHECKSUM);
		}
	}
}
//------------------------------------------------------------------------

static void NMEA_rxCR(void)
{
	char ch;

	if(getGPSData(&ch)){

		if(ch==NMEA_CHAR_CR){

			setGPSStatus(NMEA_RX_CR);

		}else{

			setGPSStatus(NMEA_FRAME_NOK);
		}
	}
}
//------------------------------------------------------------------------

static void NMEA_rxLF(void)
{
	char ch;

	if(getGPSData(&ch)){

		if(ch==NMEA_CHAR_LF){

			setGPSStatus(NMEA_RX_LF);

		}else {

			setGPSStatus(NMEA_FRAME_NOK);
		}
	}
}
//------------------------------------------------------------------------

static void NMEA_verifyFrame(void)
{
	if(NMEA_decoderFrame()){

		setGPSStatus(NMEA_FRAME_OK);

	}else{

		setGPSStatus(NMEA_FRAME_NOK);
	}
}
//------------------------------------------------------------------------

static void NMEA_acceptRxFrame(void)
{
    if(xQueueSendToBack( xQueueGPS , ( void * ) &pDataNMEA, ( TickType_t ) 1 ) ){

    	xTaskNotifyGive( xHandleMainTask );

    	setGPSStatus(NMEA_INIT_OK);
    }
}
//------------------------------------------------------------------------

static void NMEA_sendResult(void)
{

	setGPSStatus(NMEA_INIT_OK);
}
//------------------------------------------------------------------------

static void NMEA_errorRxFrame(void)
{
	setGPSStatus(NMEA_INIT_OK);
}
//------------------------------------------------------------------------

static void NMEA_errorExec(void)
{
	setGPSStatus(NMEA_INIT_OK);
}
//------------------------------------------------------------------------

static bool NMEA_decoderFrame(void){

	bool ret = false;

	List	list;

	str_split(&list, frameNMEA.Data, NMEA_CHAR_SEPARATOR);

	if(list.itens!=NULL) {

		if(list.count >= 5){

			// Checksum
			unsigned int checksum_rx;
			unsigned int checksum_calc = calcChecksum(frameNMEA.Data, frameNMEA.Count);
			checksum_rx = ~checksum_calc;
			checksum_rx = strtol(frameNMEA.checksum, NULL, 16);

			if(checksum_rx==checksum_calc) {

				char id[NMEA_LEN_ID];

				ret = true;

				AsString(id,&list,0);

				if(strcmp(GGA,id+2)==0){		decoderGGA(&list,pDataNMEA);

				}else if(strcmp(RMC,id+2)==0){	decoderRMC(&list,pDataNMEA);

				}else if(strcmp(GSA,id+2)==0){	decoderGSA(&list,pDataNMEA);

				}else{

					ret = false;
				}
			}
		}

		removeList(&list);
	}
}
//------------------------------------------------------------------------

/*
       1         2       3 4        5 6 7  8    9  10 |  12 13 14   15
	   |         |       | |        | | |  |    |  |  |  |  |   |   |
$--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh

1) Time (UTC)
2) Latitude
3) N or S (North or South)
4) Longitude
5) E or W (East or West)
6) GPS Quality Indicator,
  0 - fix not available,
  1 - GPS fix,
  2 - Differential GPS fix
7) Number of satellites in view, 00 - 12
8) Horizontal Dilution of precision
9) Antenna Altitude above/below mean-sea-level (geoid)
10) Units of antenna altitude, meters
11) Geoidal separation, the difference between the WGS-84 earth
   ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid
12) Units of geoidal separation, meters
13) Age of differential GPS data, time in seconds since last SC104
type 1 or 9 update, null field when DGPS is not used
14) Differential reference station ID, 0000-1023
15) Checksum
*/
static void decoderGGA(List* list,DataNMEA* frame){

	AsString(&frame->Identifier,			list,0);
	AsString(&frame->Time_UTC,			list,1);
	AsFloat(&frame->Lat,				list,2);
	AsChar(&frame->LatDirection,		list,3);
	AsFloat(&frame->Lng,				list,4);
	AsChar(&frame->LngDirection,		list,5);
	AsInteger(&frame->FixQuality,		list,6);
	AsInteger(&frame->NumberOfSatelites,list,7);
	AsFloat(&frame->HDOP,				list,8);
	AsFloat(&frame->Altitude,			list,9);
	AsFloat(&frame->HGeoid,				list,11);

	if(frame->LatDirection==SOUTH){

		frame->Lat *=-1;
	}

	if(frame->LngDirection == WEST){

		frame->Lng *=-1;
	}
}
//------------------------------------------------------------------------
/*
       1         2 3       4 5        6 7   8   9    10  11 12
       |         | |       | |        | |   |   |    |   |  |
$--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxx,x.x,a*hh
1) Time (UTC)
2) Status, V = Navigation receiver warning
3) Latitude
4) N or S
5) Longitude
6) E or W
7) Speed over ground, knots
8) Track made good, degrees true
9) Date, ddmmyy
10) Magnetic Variation, degrees
11) E or W
12) Checksum
*/
static void decoderRMC(List* list,DataNMEA* frame){

	AsString(&frame->Identifier,	list,0);
	AsString(&frame->Time_UTC,		list,1);
	AsChar(&frame->Status,			list,2);
	AsFloat(&frame->Lat,			list,3);
	AsChar(&frame->LatDirection,	list,4);
	AsFloat(&frame->Lng,			list,5);
	AsChar(&frame->LngDirection,	list,6);
	AsInteger(&frame->Speed,		list,7);
	AsString(&frame->Date,			list,9);
	AsFloat(&frame->MagVariation,	list,10);

	if(frame->LatDirection==SOUTH){

		frame->Lat *=-1;
	}

	if(frame->LngDirection == WEST){

		frame->Lng *=-1;
	}
}
//------------------------------------------------------------------------

/*
GSA GPS DOP and active satellites
       1 2 3                         14 15  16  17 18
       | | |                         |  |   |   |  |
$--GSA,a,a,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x.x,x.x,x.x*hh
 1) Selection mode
 2) Mode
 3) ID of 1st satellite used for fix
 4) ID of 2nd satellite used for fix
 ...
14) ID of 12th satellite used for fix
15) PDOP in meters
16) HDOP in meters
17) VDOP in meters
18) Checksum
*/
static void decoderGSA(List* list,DataNMEA* frame){

	AsString(&frame->Identifier,	list,0);
	AsChar(&frame->SelectionMode,	list,1);
	AsChar(&frame->Mode,			list,2);

	char i;
	for(i=0;i<12;i++){

		AsInteger(frame->PRNNumber+i,	list,i+3);
	}

	AsFloat(&frame->PDOP,			list,15);
	AsFloat(&frame->HDOP,			list,16);
	AsFloat(&frame->VDOP,			list,17);
}
//------------------------------------------------------------------------

static inline void setGPSStatus(StatusNMEA sts) {

	statusNMEA = sts;
}
//------------------------------------------------------------------------

inline bool getGPSData(char* ch){

	return getData(&bufferRxNMEA,ch);
}
//------------------------------------------------------------------------

inline bool putGPSData(char data) {

	return putData(&bufferRxNMEA,data);
}
//------------------------------------------------------------------------

void NMEA_init(void)
{
	clearBuffer(&bufferRxNMEA);
	clearDataNMEA(pDataNMEA);
	clearArrayFrame(&frameNMEA);
	setGPSStatus(NMEA_INIT_OK);
}
//------------------------------------------------------------------------
