/*
 * Communication.c
 *
 *  Created on: Sep 24, 2017
 *      Author: Jefferson
 */

#include "ihm.h"
#include "protocol.h"
#include "application.h"
#include "communication.h"

static const TickType_t xCommunicationDelay	= (100 / portTICK_PERIOD_MS);

TaskHandle_t 	xHandleRxTask,xHandleTxTask;

// Handles das Queues
QueueHandle_t	xQueueRx, xQueueTx;

/*
 * Pacotes recebidos
 *
 */
void rxPackage_task(){

	if(processRx()){

		// chegou pacote
	}

	vTaskDelay(xCommunicationDelay);
}
//------------------------------------------------------------------------------------

/**
 *  Pacotes a serem enviados
 *
 */
void txPackage_task(){

	uint32_t ulNotifiedValue;

	xTaskNotifyWait( 0x0, BIT_TX,  &ulNotifiedValue, portMAX_DELAY );

	if(ulNotifiedValue & BIT_TX){

		CommunicationPackage	package_tx;

		while (xQueueReceive(xQueueTx, &package_tx, (TickType_t ) 1)) {

			sendPackage(&package_tx);
		}
	}
}
//------------------------------------------------------------------------------------

void communication_init(void){

	xQueueRx	= xQueueCreate( 1, sizeof( CommunicationPackage ));
	xQueueTx	= xQueueCreate( 1, sizeof( CommunicationPackage ));

	protocol_init();
}
//------------------------------------------------------------------------------------

/**
 * Coloca pacote(cmd) recebido na fila e notifica a thread que executa callback
 *
 */
void putPackageRx(CommunicationPackage* package_rx){

	if(xQueueSendToBack( xQueueRx ,package_rx, ( TickType_t ) 1 ) ){

		xTaskNotify( xHandleCallBackTask , BIT_RX_FRAME , eSetBits );
	}
}
//------------------------------------------------------------------------------------

/**
 *
 *Coloca pacote(resposta) na fila e avisa a thread responsável pelo envio.
 *
 */

void putPackageTx(CommunicationPackage* package_tx){

	// Publica resposta na fila
	if(xQueueSendToBack( xQueueTx , package_tx, ( TickType_t ) 1 ) ){

		xTaskNotify( xHandleTxTask , BIT_TX , eSetBits );

	}else{

		// Erro ao tentar publicar a resposta
	}
}
//------------------------------------------------------------------------------------
