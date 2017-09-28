/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : LCDout.h
**     Project     : GoodsTrackerControl
**     Processor   : MKL25Z128VLK4
**     Component   : LCDHTA
**     Version     : Component 01.022, Driver 01.00, CPU db: 3.00.000
**     Repository  : My Components
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-09-25, 16:46, # CodeGen: 169
**     Abstract    :
**          This component implements a driver for multiple 2x16 character displays.
**     Settings    :
**          Component name                                 : LCDout
**          HW Interface                                   : 
**            LCD Type                                     : generic
**            LCD Lines                                    : 2
**            Characters per Line                          : 16
**            Line Addresses                               : 
**              Line 1                                     : 0x00
**              Line 2                                     : 0x40
**              Line 3                                     : 0x10
**              Line 4                                     : 0x50
**            LCD Enable Signal                            : Disabled
**            Read from Display                            : Disabled
**            Wait (us)                                    : 1000
**            E signal                                     : CtrlPin
**            E2                                           : Disabled
**            RS signal                                    : CtrlPin
**            Data/Control Bus                             : 
**              Data/Control Bus Width                     : 4bit
**              DB0..DB3                                   : Disabled
**              DB4..DB7                                   : Enabled
**                DB4                                      : DBpin
**                DB5                                      : DBpin
**                DB6                                      : DBpin
**                DB7                                      : DBpin
**              Bits/Byte Bus                              : Disabled
**          System Interface                               : 
**            Wait                                         : WAIT1
**     Contents    :
**         WriteLCDCommand  - void LCDout_WriteLCDCommand(byte cmd);
**         Write            - void LCDout_Write(char ch);
**         WriteLn          - void LCDout_WriteLn(void);
**         WriteLineStr     - void LCDout_WriteLineStr(byte line, char *str);
**         WriteString      - void LCDout_WriteString(char *str);
**         ShiftLeft        - void LCDout_ShiftLeft(void);
**         ShiftRight       - void LCDout_ShiftRight(void);
**         GotoXY           - void LCDout_GotoXY(byte line, byte column);
**         SetEntryMode     - void LCDout_SetEntryMode(bool increment, bool shiftLeft);
**         CursorShiftRight - void LCDout_CursorShiftRight(void);
**         CursorShiftLeft  - void LCDout_CursorShiftLeft(void);
**         Home             - void LCDout_Home(void);
**         Line             - void LCDout_Line(byte line);
**         Clear            - void LCDout_Clear(void);
**         UseDisplay       - byte LCDout_UseDisplay(byte display);
**
**     License   :  Open Source (LGPL)
**     Copyright : (c) Copyright Erich Styger, 2008-2014, all rights reserved.
**     http      : www.mcuoneclipse.com
**     This an open source software implementing software using Processor Expert.
**     This is a free software and is opened for education,  research  and commercial developments under license policy of following terms:
**     * This is a free software and there is NO WARRANTY.
**     * No restriction on use. You can use, modify and redistribute it for personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
**     * Redistributions of source code must retain the above copyright notice.
** ###################################################################*/
/*!
** @file LCDout.h
** @version 01.00
** @brief
**          This component implements a driver for multiple 2x16 character displays.
*/         
/*!
**  @addtogroup LCDout_module LCDout module documentation
**  @{
*/         

#ifndef __LCDout_H
#define __LCDout_H

/* MODULE LCDout. */

/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited beans */
#include "EN1.h"
#include "RS1.h"
#include "DB41.h"
#include "DB51.h"
#include "DB61.h"
#include "DB71.h"
#include "WAIT1.h"

#include "Cpu.h"

/* ID's for the soft characters which can be used as first argument for LCDout_LoadSoftChar().
  Note that ID's can start with zero, but if you want to use a zero byte in LCDout_WriteString()
  then this would be the zero delimiter byte, so not very useful. */
#define LCDout_SOFTCHAR_UE 1 /*<! ID for '�' */
#define LCDout_SOFTCHAR_AE 2 /*<! ID for '�' */
#define LCDout_SOFTCHAR_OE 3 /*<! ID for '�' */

/* support for custom soft characters in the display which can be used with LCDout_LoadSoftChar() */
extern const byte LCDout_SoftCharUE[8]; /* � */
extern const byte LCDout_SoftCharAE[8]; /* � */
extern const byte LCDout_SoftCharOE[8]; /* � */

#define LCDout_MAX_LCD_LINE_CHARS  16 /* number of chars of the LCD on a line */


void LCDout_WriteLCDData(byte ch);
/*
** ===================================================================
**     Method      :  LCDout_WriteLCDData (component LCDHTA)
**
**     Description :
**         Writes data to the display
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

void LCDout_Init(void);
/*
** ===================================================================
**     Method      :  LCDout_Init (component LCDHTA)
**
**     Description :
**         Display driver initialization routine.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

void LCDout_Clear(void);
/*
** ===================================================================
**     Method      :  LCDout_Clear (component LCDHTA)
**     Description :
**         Clears the display and moves the cursor to the first line.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_Home(void);
/*
** ===================================================================
**     Method      :  LCDout_Home (component LCDHTA)
**     Description :
**         Moves the cursor to the beginning of the first line.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_GotoXY(byte line, byte column);
/*
** ===================================================================
**     Method      :  LCDout_GotoXY (component LCDHTA)
**     Description :
**         Places the cursor on a specified position on the display.
**     Parameters  :
**         NAME            - DESCRIPTION
**         line            - Line number starting with 1
**         column          - Column number starting with 1
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_ShiftLeft(void);
/*
** ===================================================================
**     Method      :  LCDout_ShiftLeft (component LCDHTA)
**     Description :
**         Shifts all characters to the left.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

#define LCDout_Write(ch)   LCDout_WriteLCDData((byte)ch)
/*
** ===================================================================
**     Method      :  LCDout_Write (component LCDHTA)
**     Description :
**         Write a single character to the display
**     Parameters  :
**         NAME            - DESCRIPTION
**         ch              - Character to write
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_WriteLn(void);
/*
** ===================================================================
**     Method      :  LCDout_WriteLn (component LCDHTA)
**     Description :
**         Writes a new line to the display
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_WriteString(char *str);
/*
** ===================================================================
**     Method      :  LCDout_WriteString (component LCDHTA)
**     Description :
**         Writes a string to the display at the current cursor
**         position.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * str             - Pointer to string (zero byte terminated)
**                           to write to the display
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_WriteLineStr(byte line, char *str);
/*
** ===================================================================
**     Method      :  LCDout_WriteLineStr (component LCDHTA)
**     Description :
**         Writes a full line to the display (clears the rest of the
**         line).
**     Parameters  :
**         NAME            - DESCRIPTION
**         line            - Line number (starting with 1).
**       * str             - Pointer to the string which should be
**                           shown on the display.
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_Line(byte line);
/*
** ===================================================================
**     Method      :  LCDout_Line (component LCDHTA)
**     Description :
**         Sets the current line.
**     Parameters  :
**         NAME            - DESCRIPTION
**         line            - Line number, starting with 1
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_ShiftRight(void);
/*
** ===================================================================
**     Method      :  LCDout_ShiftRight (component LCDHTA)
**     Description :
**         Shifts all characters to the right.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_SetEntryMode(bool increment, bool shiftLeft);
/*
** ===================================================================
**     Method      :  LCDout_SetEntryMode (component LCDHTA)
**     Description :
**         Configures the display entry mode, if the cursor has to
**         shift and/or if the display shall shift content while
**         displaying text.
**     Parameters  :
**         NAME            - DESCRIPTION
**         increment       - Increments (TRUE) or
**                           decrements (FALSE) the display address by 1
**                           when a character code is written into or
**                           read from DDRAM. The cursor or blinking
**                           moves to the right when incremented by 1
**                           and to the left when decremented by 1.
**         shiftLeft       - The display does not shift if
**                           the 'shift' is FALSE. If 'shift' is TRUE,
**                           it will seem as if the cursor does not move
**                           but the display does.
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_WaitForLCDReady(void);
/*
** ===================================================================
**     Method      :  LCDout_WaitForLCDReady (component LCDHTA)
**
**     Description :
**         Waits until the display is ready
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

byte LCDout_UseDisplay(byte display);
/*
** ===================================================================
**     Method      :  LCDout_UseDisplay (component LCDHTA)
**     Description :
**         
**     Parameters  :
**         NAME            - DESCRIPTION
**         display         - Has to be either 1 (top display,
**                           using E1) or 2 (bottom display, using E2)
**     Returns     :
**         ---             - Error code
** ===================================================================
*/

void LCDout_CursorShiftRight(void);
/*
** ===================================================================
**     Method      :  LCDout_CursorShiftRight (component LCDHTA)
**     Description :
**         Shift the cursor to the right.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_CursorShiftLeft(void);
/*
** ===================================================================
**     Method      :  LCDout_CursorShiftLeft (component LCDHTA)
**     Description :
**         Shift the cursor to the left.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LCDout_WriteLCDCommand(byte cmd);
/*
** ===================================================================
**     Method      :  LCDout_WriteLCDCommand (component LCDHTA)
**     Description :
**         Writes a command to the display
**     Parameters  :
**         NAME            - DESCRIPTION
**         cmd             - command passed to the LCD
**     Returns     : Nothing
** ===================================================================
*/

/* END LCDout. */

#endif
/* ifndef __LCDout_H */
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
