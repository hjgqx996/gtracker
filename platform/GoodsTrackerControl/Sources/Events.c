/* ###################################################################
**     Filename    : Events.c
**     Project     : GoodsTrackerControl
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-06-20, 13:54, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

#include "protocol.h"

/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MKL25Z128LK4]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  TSSin_fOnFault (module Events)
**
**     Component   :  TSSin [TSS_Library]
**     Description :
**         This callback function is called by TSS after Fault
**         occurence. This event is enabled always and depends on
**         selection 'generate code' if the callback is used.
**         The default CallBack Name is automatically generated with
**         automatic prefix update by current Component Name. User can
**         define own name, but then the automatic name update is not
**         functional.
**         Option is available from TSS3.0 version.
**     Parameters  :
**         NAME            - DESCRIPTION
**         u8FaultElecNum  - The value defines
**                           electrode number on which measurement fault
**                           occured.
**     Returns     : Nothing
** ===================================================================
*/
void TSSin_fOnFault(byte u8FaultElecNum)
{
  #if TSS_USE_PE_COMPONENT
  /* If large or small capacitor fault  */
  if(tss_CSSys.Faults.ChargeTimeout || tss_CSSys.Faults.SmallCapacitor)
  {
    /* Write your code here ... */
  }
  /* If data corruption fault  */
  if(tss_CSSys.Faults.DataCorruption)
  {
    /* Write your code here ... */
  }
  /* If small trigger period fault  */
  if(tss_CSSys.Faults.SmallTriggerPeriod)
  {
    /* Write your code here ... */
  }
  /* Clear the fault flags */
  (void) TSS_SetSystemConfig(System_Faults_Register, 0x00);

  (void) u8FaultElecNum;
  return;
  #endif
}

/*
** ===================================================================
**     Event       :  TSSin_fOnInit (module Events)
**
**     Component   :  TSSin [TSS_Library]
**     Description :
**         This callback function is automatically called during the
**         TSS Init function execution. The function is intended for
**         implementation of peripherals initialization. TSS Component
**         automatically enables clock for all used TSS peripherals in
**         the internal function TSS_InitDevices which is called by
**         this callback.
**         This event is enabled always and depends on selection
**         'generate code' if the callback is used.
**         The default CallBack Name is automatically generated with
**         automatic prefix update by current Component Name. User can
**         define own name, but then the automatic name update is not
**         functional.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void TSSin_fOnInit(void)
{
  #if TSS_USE_PE_COMPONENT
    TSSin_InitDevices();
  #endif

  /* Write your code here ... */

}

/*
** ===================================================================
**     Event       :  TSSin_fCallBack0 (module Events)
**
**     Component   :  TSSin [TSS_Library]
**     Description :
**         Callback definition for Control 0. This event is enabled
**         only if Control 0 is enabled.
**         The default CallBack Name is automatically generated with
**         automatic prefix update by current Component Name. User can
**         define own name, but then the automatic name update is not
**         functional.
**     Parameters  :
**         NAME            - DESCRIPTION
**         u8ControlId     - Valid unique Identifier of
**                           the Control which generated the CallBack
**                           function. This Id can be used for finding
**                           of Callback's source Control.
**     Returns     : Nothing
** ===================================================================
*/
void TSSin_fCallBack0(TSS_CONTROL_ID u8ControlId)
{
    ihm_put_slide_event(&TSSin_cKey0);

#if 0
  if (TSSin_cKey0.DynamicStatus.Movement)
  {
    if (TSSin_cKey0.Events.Touch)
    {
      if (!(TSSin_cKey0.Events.InvalidPos))
      {
        (void) TSSin_cKey0.Position;

        (void)u8ControlId; /* avoid warning */



        //LED_R_Put(TSSin_cKey0.Position<=30);
        //LED_G_Put(TSSin_cKey0.Position>30 && TSSin_cKey0.Position<=35);
        //LED_B_Put(TSSin_cKey0.Position>35 && TSSin_cKey0.Position<=64);
      }
    }
    else
    {
      /* Write your code here ... */
    }
  }
#endif

  (void) u8ControlId;
  return;
}

/*
** ===================================================================
**     Event       :  GT_TI1_OnInterrupt (module Events)
**
**     Component   :  GT_TI1 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void GT_TI1_OnInterrupt(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  GT_AD1_OnEnd (module Events)
**
**     Component   :  GT_AD1 [ADC]
**     Description :
**         This event is called after the measurement (which consists
**         of <1 or more conversions>) is/are finished.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void GT_AD1_OnEnd(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  GT_AD1_OnCalibrationEnd (module Events)
**
**     Component   :  GT_AD1 [ADC]
**     Description :
**         This event is called when the calibration has been finished.
**         User should check if the calibration pass or fail by
**         Calibration status method./nThis event is enabled only if
**         the <Interrupt service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void GT_AD1_OnCalibrationEnd(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  GT_AsyncSerial_OnError (module Events)
**
**     Component   :  GT_AsyncSerial [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void GT_AsyncSerial_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  GT_AsyncSerial_OnRxChar (module Events)
**
**     Component   :  GT_AsyncSerial [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void GT_AsyncSerial_OnRxChar(void)
{
	char byte = 0;

	if(GT_AsyncSerial_RecvChar(&byte)==ERR_OK){

		putRxData(byte);
	}

	timeRx 	= TIME_RX;
}

/*
** ===================================================================
**     Event       :  GT_AsyncSerial_OnTxChar (module Events)
**
**     Component   :  GT_AsyncSerial [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void GT_AsyncSerial_OnTxChar(void)
{
	static char byte		= 0;
	static char tx_result	= ERR_OK;

	if(tx_result == ERR_OK){

		if(getTxData(&byte)){

			tx_result = GT_AsyncSerial_SendChar(byte);
		}

	}else{

		tx_result = GT_AsyncSerial_SendChar(byte);
	}

	timeTx 	= TIME_TX;
}

/*
** ===================================================================
**     Event       :  GT_FRTOS_vApplicationStackOverflowHook (module Events)
**
**     Component   :  GT_FRTOS [FreeRTOS]
**     Description :
**         if enabled, this hook will be called in case of a stack
**         overflow.
**     Parameters  :
**         NAME            - DESCRIPTION
**         pxTask          - Task handle
**       * pcTaskName      - Pointer to task name
**     Returns     : Nothing
** ===================================================================
*/
void GT_FRTOS_vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
  /* This will get called if a stack overflow is detected during the context
     switch.  Set configCHECK_FOR_STACK_OVERFLOWS to 2 to also check for stack
     problems within nested interrupts, but only do this for debug purposes as
     it will increase the context switch time. */
  (void)pxTask;
  (void)pcTaskName;
  taskDISABLE_INTERRUPTS();
  /* Write your code here ... */
  for(;;) {}
}

/*
** ===================================================================
**     Event       :  GT_FRTOS_vApplicationMallocFailedHook (module Events)
**
**     Component   :  GT_FRTOS [FreeRTOS]
**     Description :
**         If enabled, the RTOS will call this hook in case memory
**         allocation failed.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void GT_FRTOS_vApplicationMallocFailedHook(void)
{
  /* Called if a call to pvPortMalloc() fails because there is insufficient
     free memory available in the FreeRTOS heap.  pvPortMalloc() is called
     internally by FreeRTOS API functions that create tasks, queues, software
     timers, and semaphores.  The size of the FreeRTOS heap is set by the
     configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
  taskDISABLE_INTERRUPTS();
  /* Write your code here ... */
  for(;;) {}
}

/*
** ===================================================================
**     Event       :  GT_FRTOS_vApplicationTickHook (module Events)
**
**     Component   :  GT_FRTOS [FreeRTOS]
**     Description :
**         If enabled, this hook will be called by the RTOS for every
**         tick increment.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void GT_FRTOS_vApplicationTickHook(void)
{
  /* Called for every RTOS tick. */
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  GT_FRTOS_vApplicationIdleHook (module Events)
**
**     Component   :  GT_FRTOS [FreeRTOS]
**     Description :
**         If enabled, this hook will be called when the RTOS is idle.
**         This might be a good place to go into low power mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void GT_FRTOS_vApplicationIdleHook(void)
{
  /* Called whenever the RTOS is idle (from the IDLE task).
     Here would be a good place to put the CPU into low power mode. */
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  Cpu_OnSupervisorCall (module Events)
**
**     Component   :  Cpu [MKL25Z128LK4]
*/
/*!
**     @brief
**         This event is called when the Supervisor Call exception had
**         occurred. This event is automatically enabled when the
**         [Supervisor Call] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnSupervisorCall(void)
{
	vPortSVCHandler();
}

/*
** ===================================================================
**     Event       :  Cpu_OnPendableService (module Events)
**
**     Component   :  Cpu [MKL25Z128LK4]
*/
/*!
**     @brief
**         This event is called when the Pendable Service exception had
**         occurred. This event is automatically enabled when the
**         [Pendable Service] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnPendableService(void)
{
	vPortPendSVHandler();
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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