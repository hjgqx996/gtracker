/*
 * application.c
 *
 *  Created on: 04/06/2017
 *      Author: Jefferson
 */

#include <stdlib.h>
#include <stdbool.h>
#include <Telemetria.h>

#include "AppQueues.h"
#include "clock.h"
#include "lock.h"
#include "NMEAFrame.h"
#include "Level.h"
#include "Accelerometer.h"
#include "Serialization.h"
#include "application.h"

static 	int			_lock;
static 	CommunicationFrame	Answer,Cmd;

void runApp(void){

	uint32_t ulNotifiedValue;

	xTaskNotifyWait( 0x0, BIT_RX_FRAME | BIT_UPDATE_GPS | BIT_UPDATE_ACCE | BIT_UPDATE_AD,  &ulNotifiedValue, portMAX_DELAY);

	updateTLM(ulNotifiedValue);

	execCMD(ulNotifiedValue);
}
//-------------------------------------------------------------------------

void runMain(void){

// TODO
}
//-------------------------------------------------------------------------

void execCMD(uint32_t ulNotifiedValue){

	if(ulNotifiedValue & BIT_RX_FRAME){

		while (xQueueReceive(xQueueCom, &Cmd, (TickType_t ) 1)) {

			pCallBack cb = getCallBack(Cmd.resource);

			if(cb!=NULL){

				//Preenche dados do Header para retorno
				setHeaderAnswer(&Cmd);

				if(cb(&Cmd) == CMD_RESULT_EXEC_SUCCESS) {

					// Publica resposta na fila
					if(xQueueSendToBack( xQueueAnswer , &Answer, ( TickType_t ) 1 ) ){

				    	xTaskNotify( xHandleRunTxTask , BIT_TX , eSetBits );

					}else{

						// Erro ao tentar publicar a resposta
					}
				}
				else {

					// Sem sucesso na execução do comando
				}
			}
		}
	}
}
//-------------------------------------------------------------------------

ResultExec onLED(CommunicationFrame* frame){

	ResultExec res = CMD_RESULT_EXEC_UNSUCCESS;

	AppendPayLoad(&Answer.PayLoad,"1569695954");

	res = CMD_RESULT_EXEC_SUCCESS;

	return res;
}
//-------------------------------------------------------------------------

ResultExec onAnalog(CommunicationFrame* cmd){

	ResultExec res = CMD_RESULT_EXEC_UNSUCCESS;

	AppendPayLoad(&Answer.PayLoad,"1569695954");;

	res = CMD_RESULT_EXEC_SUCCESS;

	return res;
}
//-------------------------------------------------------------------------

ResultExec onAccel(CommunicationFrame* cmd){

	ResultExec res = CMD_RESULT_EXEC_UNSUCCESS;

	AppendPayLoad(&Answer.PayLoad,"1569695954");;

	res = CMD_RESULT_EXEC_SUCCESS;

	return res;
}
//-------------------------------------------------------------------------

ResultExec onTouch(CommunicationFrame* cmd){

	ResultExec res = CMD_RESULT_EXEC_UNSUCCESS;

	AppendPayLoad(&Answer.PayLoad,"1569695954");;

	res = CMD_RESULT_EXEC_SUCCESS;

	return res;
}
//-------------------------------------------------------------------------

ResultExec onPWM(CommunicationFrame* cmd){

	ResultExec res = CMD_RESULT_EXEC_UNSUCCESS;

	AppendPayLoad(&Answer.PayLoad,"1569695954");;

	res = CMD_RESULT_EXEC_SUCCESS;

	return res;
}
//------------------------------------------------------------------------

ResultExec onTelemetry(CommunicationFrame* cmd){

	ResultExec res = CMD_RESULT_EXEC_UNSUCCESS;

	tlm2String(&telemetria,&Answer.PayLoad);

	res = CMD_RESULT_EXEC_SUCCESS;

	return res;
}
//------------------------------------------------------------------------

ResultExec onLock(CommunicationFrame* cmd){

	ResultExec res = CMD_RESULT_EXEC_UNSUCCESS;

	decoderLockPayLoad(&cmd->PayLoad);

	if(_lock){

		lock();

	}else{

		unLock();
	}

	AppendPayLoad(&Answer.PayLoad,"1569695954");;

	res = CMD_RESULT_EXEC_SUCCESS;

	return res;
}
//------------------------------------------------------------------------

void decoderLockPayLoad(PayLoad* payload){

	AsInteger(&_lock,payload->Data,0,CHAR_SEPARATOR);
}
//------------------------------------------------------------------------

/*
 *
 * Set endereco de origem e destino e o tipo da operacao
 *
 */
static void setHeaderAnswer(CommunicationFrame* data){

	clearArrayPayLoad(&Answer.PayLoad);

	strcpy(Answer.operacao,OPERATION_AN);
	Answer.resource		= data->resource;
	Answer.dest			= data->address;
	Answer.address		= ADDRESS;
	Answer.time_stamp	= getCurrentTimeStamp();
}
//------------------------------------------------------------------------

pCallBack getCallBack(Resource r) {

	pCallBack	cb = NULL;

	switch(r){

		case CMD_LED:		cb = onLED;			break;
		case CMD_ANALOG:	cb = onAnalog;		break;
		case CMD_PWM:		cb = onPWM;			break;
		case CMD_ACC:		cb = onAccel;		break;
		case CMD_TOUCH:		cb = onTouch;		break;
		case CMD_TLM:		cb = onTelemetry;	break;
		case CMD_LOCK:		cb = onLock;		break;
		case CMD_LCD:		cb = NULL;			break;
	}

	return cb;
}
//------------------------------------------------------------------------

void initApp(void){

	clearTelemetria(&telemetria);
	clearData(&Answer);
}
//------------------------------------------------------------------------
