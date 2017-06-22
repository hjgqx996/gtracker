#include <stdlib.h>
#include <stdio.h>

#include "RingBuffer.h"
#include "utils.h"
#include "protocol.h"

unsigned int timeTx = TIME_TX;
unsigned int timeRx = TIME_RX;

StatusRx	statusRx = CMD_INIT;
RingBuffer	bufferRx,bufferTx;
DataFrame	dataFrame;

Cmd			ListCmd[]	= {	{.id_cmd=CMD_LED,		.resource = "LED\0",	.cb=NULL},
							{.id_cmd=CMD_ANALOG,	.resource = "AN\0",		.cb=NULL},
							{.id_cmd=CMD_PWM,		.resource = "PWM\0",	.cb=NULL},
							{.id_cmd=CMD_ACC,		.resource = "ACC\0",	.cb=NULL},
							{.id_cmd=CMD_TOUCH,		.resource = "TOU\0",	.cb=NULL},
							{.id_cmd=CMD_TELEMETRIA,.resource = "TLM\0",	.cb=NULL},
							{.id_cmd=CMD_LOCK,		.resource = "LCK\0",	.cb=NULL},
							};

const unsigned char SIZE_LIST_CMD = sizeof(ListCmd)/sizeof(Cmd);

void processProtocol(void) {

	switch(statusRx){

		default:
		case CMD_INIT:			initRxCMD();			break;
		case CMD_INIT_OK:		rxStartCMD();			break;
		case CMD_RX_START:		receiveFrame();			break;
		case CMD_RX_PAYLOAD:	receiveFrame();			break;
		case CMD_RX_END:		rxCR();					break;
		case CMD_RX_CR:			rxNL();					break;
		case CMD_RX_NL:			decoderCMD();			break;
		case CMD_DECODER:		execCallBack();			break;

		case CMD_ERROR:			errorRx();				break;
		case CMD_ACK:			sendACK();				break;
		case CMD_NAK:			sendNAK();				break;
		case CMD_EXEC:			sendResult();			break;
	}

	startTX();
}
//------------------------------------------------------------------------

void rxStartCMD (void) {

	char ch;

	if(getRxData(&ch)){

		if(ch==CHAR_CMD_START){

			clearData(&dataFrame);

			setStatusRx(CMD_RX_START);
		}
	}
}
//------------------------------------------------------------------------

void receiveFrame (void) {

	char ch;

	if(getRxData(&ch)){

		dataFrame.checksum_calc ^= ch;

		if(ch==CHAR_CMD_START || dataFrame.sizeFrame>=LEN_MAX_FRAME){

			setStatusRx(CMD_ERROR);

		}else if(ch==CHAR_CMD_END){

			if(dataFrame.sizeFrame>=LEN_MIN_FRAME){

				setStatusRx(CMD_RX_END);

			}else{

				setStatusRx(CMD_ERROR);
			}

		}else {

			setStatusRx(CMD_RX_PAYLOAD);

			dataFrame.frame[(dataFrame.sizeFrame++)%LEN_MAX_FRAME] = ch;
		}
	}
}
//------------------------------------------------------------------------

void rxNL(void) {

	char ch;

	if(getRxData(&ch)){

		if(ch==CHAR_LF){

			setStatusRx(CMD_RX_NL);

		}else {

			setStatusRx(CMD_ERROR);
		}
	}
}
//------------------------------------------------------------------------

void rxCR(void) {

	char ch;

	if(getRxData(&ch)){

		if(ch==CHAR_CR){

			setStatusRx(CMD_RX_CR);

		}else{

			setStatusRx(CMD_ERROR);
		}
	}
}
//------------------------------------------------------------------------

void decoderCMD(void) {

//	formatCMD();

	if(decoderFrame2()){

		setStatusRx(CMD_DECODER);

	}else{

		setStatusRx(CMD_ERROR);
	}
}
//------------------------------------------------------------------------


bool decoderFrame2(void) {

	List	list;

	str_split(&list,dataFrame.frame,CHAR_SEPARATOR);

	if(list.itens!=NULL){

		dataFrame.checksum_rx = atoi(list.itens[list.size-1]);

		if(dataFrame.checksum_rx==dataFrame.checksum_calc){

			char i;

			for(i=0;i < list.size;i++){

				if(list.itens[i]!=NULL){

					switch(i){

						case 0:	dataFrame.dest				= atoi(list.itens[0]);				break;
						case 1:	dataFrame.address			= atoi(list.itens[1]);				break;
						case 2:	strncpy(dataFrame.operacao, list.itens[2],2);					break;
						case 3:	strncpy(dataFrame.resource,	list.itens[3],3);					break;
						case 4:	dataFrame.sizePayLoad		= atoi(list.itens[4]);				break;
						case 5:	strncpy(dataFrame.payload, 	list.itens[5],dataFrame.sizePayLoad);	break;
					}
				}
			}
		}

		removeList(&list);
	}

	return list.size>=2;
}
//------------------------------------------------------------------------

void setEventCMD(IdCmd id_cmd,pCallBack c) {
	char i;
	for(i=0;i<SIZE_LIST_CMD;i++){

		if(ListCmd[i].id_cmd==id_cmd){

			ListCmd[i].cb = c;
			break;
		}
	}
}
//------------------------------------------------------------------------

pCallBack getCallBack(void) {

	pCallBack cb = NULL;
	char i;
	for(i=0;i < SIZE_LIST_CMD;i++){

		if(strcmp(ListCmd[i].resource,dataFrame.resource)==0){

			cb = ListCmd[i].cb;
			break;
		}
	}

	return cb;
}
//------------------------------------------------------------------------

void execCallBack(void) {

	pCallBack cb = getCallBack();

	if(cb!=NULL && cb(&dataFrame)==EXEC_SUCCESS){

//		if(*dataFrame.frame!=CHAR_STR_END){

			setStatusRx(CMD_EXEC);

//		}else {

//			setStatusRx(CMD_ACK);
//		}

	}else{

		setStatusRx(CMD_NAK);
	}
}
//------------------------------------------------------------------------

void initRxCMD(void) {

	clearData(&dataFrame);
	initBuffer(&bufferRx);
	initBuffer(&bufferTx);
	setStatusRx(CMD_INIT_OK);
}
//------------------------------------------------------------------------

void sendACK(void) {

	sprintf(dataFrame.frame,"[%s%d%d]",dataFrame.resource,dataFrame.address,dataFrame.value);

	sendString(dataFrame.frame);

	setStatusRx(CMD_INIT_OK);
}
//------------------------------------------------------------------------

void sendNAK(void) {

	sprintf(dataFrame.frame,"[%s%d%d]%s",dataFrame.resource,dataFrame.address,dataFrame.value,CHAR_NAK);

	sendString(dataFrame.frame);

	setStatusRx(CMD_INIT_OK);
}
//------------------------------------------------------------------------

void sendResult(void){

	dataFrame.dest = dataFrame.address;
	dataFrame.address = ADDRESS;


	buildHeader(&dataFrame);

	sendString(dataFrame.frame);

	setStatusRx(CMD_INIT_OK);
}
//------------------------------------------------------------------------

void sendString(const char* str){

	char* p = (char *)str;

	if(p!=NULL){

		while(*p!=CHAR_STR_END){

			putTxData(*p++);
		}

		putTxData(CHAR_CR);
		putTxData(CHAR_LF);
	}
}
//------------------------------------------------------------------------

inline void setStatusRx(StatusRx sts) {

	statusRx = sts;
}
//------------------------------------------------------------------------

inline bool putTxData(char data) {

	return putData(&bufferTx,data);
}
//------------------------------------------------------------------------

inline bool putRxData(char data) {

	return putData(&bufferRx,data);
}
//------------------------------------------------------------------------

inline bool getTxData(char* ch){

	return getData(&bufferTx,ch);
}
//------------------------------------------------------------------------

inline bool getRxData(char* ch){

	return getData(&bufferRx,ch);
}
//------------------------------------------------------------------------

void errorRx(void){

	/**/

	setStatusRx(CMD_INIT_OK);
}
//------------------------------------------------------------------------

void clearData(DataFrame* frame){

	frame->checksum_rx	= -1;
	frame->checksum_calc= 0;
	frame->address		= 0;
	frame->value		= 0;
	frame->sizeFrame	= 0;
	frame->sizePayLoad	= 0;

	int i;

	for(i=0;i<LEN_MAX_PAYLOAD;i++){

		frame->payload[i]	= CHAR_STR_END;
	}

	for(i=0;i<LEN_MAX_FRAME;i++){

		frame->frame[i]		= CHAR_STR_END;
	}

}
//------------------------------------------------------------------------

bool hasTxData(void){

	return hasData(&bufferTx);
}
//------------------------------------------------------------------------

/**
 * Verifica se o buffer de TX esta vazio. Se sim, chama a call back
 * da transmissao de caracter para iniciar a transmissao do buffer
 *
 */
void startTX(void){

	if(hasTxData() && GT_AsyncSerial_GetCharsInTxBuf()==0){

		GT_AsyncSerial_OnTxChar();
	}
}
//------------------------------------------------------------------------

void buildHeader(DataFrame *frame){

	sprintf(frame->frame,"%05d%05d:%s:%s:%03d",	frame->address,
												frame->dest,
												frame->operacao,
												frame->resource,
												frame->sizePayLoad);

}
//------------------------------------------------------------------------

void setPayLoad(DataFrame* frame,char* str){

	frame->sizePayLoad = (int)strlen(str);

	strncpy(frame->payload,str,frame->sizePayLoad);
	strncpy(frame->frame+SIZE_HEADER,frame->payload,frame->sizePayLoad);
}
//------------------------------------------------------------------------