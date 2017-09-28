/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.5 [05.21]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */

/* Begin of <includes> initialization, DO NOT MODIFY LINES BELOW */

#include "TSK1.h"
#include "FRTOS1.h"
#include "frtos_tasks.h"

/* End <includes> initialization, DO NOT MODIFY LINES ABOVE */

#include "application.h"
#include "ihm.h"
#include "clock.h"
#include "serial.h"
#include "communication.h"
#include "accelerometer.h"
#include "gps.h"

TaskHandle_t xHandleMainTask;

static const TickType_t xMainDelay		= (200 / portTICK_PERIOD_MS);
static const TickType_t xTankDelay		= (200 / portTICK_PERIOD_MS);
static const TickType_t xGPSDelay 		= (200 / portTICK_PERIOD_MS);
static const TickType_t xAcceDelay		= (200 / portTICK_PERIOD_MS);
static const TickType_t xRxComDelay		= (200 / portTICK_PERIOD_MS);

/*********************************************************************************************
  * Main task
  * Espera as notificacoes para atualizacoes das informacoes de telemtria
  *
  ********************************************************************************************/

static portTASK_FUNCTION(run_main, pvParameters) {

	tank_init();

	ihm_init();

	gps_init();

	accelerometer_init();

	clock_init();

	uart_init();

	communication_init();

	app_init();

	while(1) {

		ihm_notify_screen_stat();

		readKey();

		vTaskDelay(xMainDelay);
	}

	vTaskDelete(xHandleMainTask);
}

/*********************************************************************************************
 *  Leitura dos canais analogicos e digitais
 *
 ********************************************************************************************/

static portTASK_FUNCTION(run_data, pvParameters) {

	while(1) {

		tank_task();

		vTaskDelay(xTankDelay);
	}

	vTaskDelete(xHandleDataTask);
}

/*********************************************************************************************
 *  Gerencimento do IHM LCD-Touch
 *
 ********************************************************************************************/

static portTASK_FUNCTION(run_ihm, pvParameters) {

	while(1) {

		ihm_task();
	}

	deInitIHM();

	vTaskDelete(xHandleIHMTask);
}


/*********************************************************************************************
 *  Task de gerenciamento do GPS/NMEA
 *
 *********************************************************************************************/

static portTASK_FUNCTION(run_gps, pvParameters) {

	while(1) {

		gps_task();

		vTaskDelay(xGPSDelay);
	}

	vTaskDelete(xHandleGPSTask);
}

/*********************************************************************************************
 *
 *  Task leitura do acelerometro
 *
 ********************************************************************************************/

static portTASK_FUNCTION(run_accel, pvParameters) {

	while(1) {

		accelerometer_task();

		vTaskDelay(xAcceDelay);
	}

	vTaskDelete(xHandleAccelTask);
}

/*********************************************************************************************
 *  Task para execucao da call back dos cmds recebidos
 *  O comando de telemetria espera as notificacoes para atualizacoes das informacoes via fila de mensagens
 *
 *********************************************************************************************/

static portTASK_FUNCTION(run_callback, pvParameters) {

	while(1) {

		callback_task();
	}

	vTaskDelete(xHandleCallBackTask);
}

/*********************************************************************************************
 * Task para processamento da recepcao dos dados
 *
 *********************************************************************************************/

static portTASK_FUNCTION(run_RX, pvParameters) {

	while(1) {

		rxPackage_task();

		vTaskDelay(xRxComDelay);
	}

	vTaskDelete(xHandleRxTask);
}

/*********************************************************************************************
 *  Task para processamento das transmissao
 *
 ********************************************************************************************/

static portTASK_FUNCTION(run_TX, pvParameters) {

	while(1) {

		txPackage_task();
	}

	vTaskDelete(xHandleTxTask);
}

/*********************************************************************************************
 *  Funcao para criacao das tasks
 *
 ********************************************************************************************/

void CreateTasks(void) {

	if (FRTOS1_xTaskCreate(
			run_main, /* pointer to the task */
			"run_main", /* task name for kernel awareness debugging */
			configMINIMAL_STACK_SIZE, /* task stack size */
			(void*)NULL, /* optional task startup argument */
			tskIDLE_PRIORITY + 10, /* initial priority */
			&xHandleMainTask /* optional task handle to create */
	) != pdPASS) {
		/*lint -e527 */
		for (;;) {
		}; /* error! probably out of memory */
		/*lint +e527 */
	}
	if (FRTOS1_xTaskCreate(
			run_RX, /* pointer to the task */
			"run_RX", /* task name for kernel awareness debugging */
			configMINIMAL_STACK_SIZE+50, /* task stack size */
			(void*)NULL, /* optional task startup argument */
			tskIDLE_PRIORITY + 0, /* initial priority */
			&xHandleRxTask /* optional task handle to create */
	) != pdPASS) {
		/*lint -e527 */
		for (;;) {
		}; /* error! probably out of memory */
		/*lint +e527 */
	}
	if (FRTOS1_xTaskCreate(
			run_data, /* pointer to the task */
			"run_data", /* task name for kernel awareness debugging */
			configMINIMAL_STACK_SIZE + 0, /* task stack size */
			(void*)NULL, /* optional task startup argument */
			tskIDLE_PRIORITY + 0, /* initial priority */
			&xHandleDataTask /* optional task handle to create */
	) != pdPASS) {
		/*lint -e527 */
		for (;;) {
		}; /* error! probably out of memory */
		/*lint +e527 */
	}
	if (FRTOS1_xTaskCreate(
			run_ihm, /* pointer to the task */
			"run_ihm", /* task name for kernel awareness debugging */
			configMINIMAL_STACK_SIZE + 50, /* task stack size */
			(void*)NULL, /* optional task startup argument */
			tskIDLE_PRIORITY + 3, /* initial priority */
			&xHandleIHMTask /* optional task handle to create */
	) != pdPASS) {
		/*lint -e527 */
		for (;;) {
		}; /* error! probably out of memory */
		/*lint +e527 */
	}

	if (FRTOS1_xTaskCreate(
			run_gps, /* pointer to the task */
			"run_gps", /* task name for kernel awareness debugging */
			configMINIMAL_STACK_SIZE + 0, /* task stack size */
			(void*)NULL, /* optional task startup argument */
			tskIDLE_PRIORITY + 4, /* initial priority */
			&xHandleGPSTask /* optional task handle to create */
	) != pdPASS) {
		/*lint -e527 */
		for (;;) {
		}; /* error! probably out of memory */
		/*lint +e527 */
	}

	if (FRTOS1_xTaskCreate(
			run_accel, /* pointer to the task */
			"run_accel", /* task name for kernel awareness debugging */
			configMINIMAL_STACK_SIZE + 0, /* task stack size */
			(void*)NULL, /* optional task startup argument */
			tskIDLE_PRIORITY + 0, /* initial priority */
			&xHandleAccelTask /* optional task handle to create */
	) != pdPASS) {
		/*lint -e527 */
		for (;;) {
		}; /* error! probably out of memory */
		/*lint +e527 */
	}

	if (FRTOS1_xTaskCreate(
			run_callback, /* pointer to the task */
			"run_callback", /* task name for kernel awareness debugging */
			configMINIMAL_STACK_SIZE + 50, /* task stack size */
			(void*)NULL, /* optional task startup argument */
			tskIDLE_PRIORITY + 5, /* initial priority */
			&xHandleCallBackTask /* optional task handle to create */
	) != pdPASS) {
		/*lint -e527 */
		for (;;) {
		}; /* error! probably out of memory */
		/*lint +e527 */
	}

	if (FRTOS1_xTaskCreate(
			run_TX, /* pointer to the task */
			"run_TX", /* task name for kernel awareness debugging */
			configMINIMAL_STACK_SIZE+50, /* task stack size */
			(void*)NULL, /* optional task startup argument */
			tskIDLE_PRIORITY + 0, /* initial priority */
			&xHandleTxTask /* optional task handle to create */
	) != pdPASS) {
		/*lint -e527 */
		for (;;) {
		}; /* error! probably out of memory */
		/*lint +e527 */
	}
}
