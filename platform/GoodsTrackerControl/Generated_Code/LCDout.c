/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : LCDout.c
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
** @file LCDout.c
** @version 01.00
** @brief
**          This component implements a driver for multiple 2x16 character displays.
*/         
/*!
**  @addtogroup LCDout_module LCDout module documentation
**  @{
*/         

/* MODULE LCDout. */

#include "LCDout.h"


/* DEFINES for display commands.
   See
   - www.freescale.com/files/microcontrollers/doc/app_note/AN1745.pdf
   - http://www.mikrocontroller.net/articles/HD44780
   for additional details.
*/
#define ClearDisplayCmd                 0x01 /* clears the display */

#define ReturnHomeCmd                   0x02 /* moves the cursor to the beginning of the first line */

#define EntryModeSetCmd                 0x04
  #define EntryModeSet_ShiftOn             1 /* S flag: shift display */
  #define EntryModeSet_IncrementOn         2 /* I/D flag: increment cursor */

#define DisplayOnOffControlCmd          0x08 /* Display on/off control command. There are 3 bits D, C and B as well */
  #define DisplayOnOffControl_BlinkOn      1 /* B flag: blinking cursor on/off; B=1 blinking, B=0 not blinking */
  #define DisplayOnOffControl_CursorOn     2 /* C flag: cursor on/off, C=1 cursor on, C=0 cursor off */
  #define DisplayOnOffControl_DisplayOn    4 /* D flag: display on/off, D=1 display on, D=0 display off */

#define FunctionSetCmd                  0x20
  #define FunctionSet_8bit              0x10 /* DL flag: DL=1: 8bit, DL=0: 4bit */
  #define FunctionSet_4bit                 0 /* DL flag: DL=1: 8bit, DL=0: 4bit */
  #define FunctionSet_2Lines            0x08 /* N flag: number of display lines: N=1 2 or 4 lines, N=0 1 line */
  #define FunctionSet_1Line                0 /* N flag: number of display lines: N=1 2 or 4 lines, N=0 1 line */
  #define FunctionSet_Font5x10          0x04 /* F flag: character font, F=1 5x10 dots, F=0 5x8 dots */
  #define FunctionSet_Font5x8              0 /* F flag: character font, F=1 5x10 dots, F=0 5x8 dots */

#define CursorOnCmd    (DisplayOnOffControlCmd|DisplayOnOffControl_DisplayOn|DisplayOnOffControl_CursorOn)
#define CursorOffCmd   (DisplayOnOffControlCmd|DisplayOnOffControl_DisplayOn)
#define GotoXYCmd      0x80  /* 0x80 | Display RAM address */
#define Line1Offset    0x00 /* Display RAM address of first line, usually 0x00 */
#define Line2Offset    0x40 /* Display RAM address of second line, usually 0x40 */
#define Line3Offset    0x10 /* Display RAM address of third line, usually 0x10 */
#define Line4Offset    0x50 /* Display RAM address of fourth line, usually 0x50 */
#define FirstLineCmd   (GotoXYCmd|Line1Offset)
#define SecondLineCmd  (GotoXYCmd|Line2Offset)
#define ThirdLineCmd   (GotoXYCmd|Line3Offset)
#define FourthLineCmd  (GotoXYCmd|Line4Offset)

#define RightShiftCmd      0x1C
#define LeftShiftCmd       0x18
#define RightShiftCursor   0x14
#define LeftShiftCursor    0x10

#define BusyFlag       0x80 /* BF Flag */

/* support for custom soft characters in the display which can be used with LCDout_LoadSoftChar() */
const byte LCDout_SoftCharUE[8] = { /* � */
  0x11, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0d, 0
/* X...X
   .....
   X...X
   X...X
   X...X
   X..XX
   .XX.X */
};

const byte LCDout_SoftCharAE[8] = { /* � */
  0x11, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0
/* X...X
   .....
   .XXX.
   ....X
   .XXXX
   X...X
   .XXXX */
};

const byte LCDout_SoftCharOE[8] = { /* � */
  0x11, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E, 0
/* X...X
   .....
   .XXX.
   X...X
   X...X
   X...X
   .XXX. */
};

/* macros for the RS pin */
#define ClrRS() \
        RS1_ClrVal()                    /* RS=0: command mode */
#define SetRS() \
        RS1_SetVal()                    /* RS=1: data mode */

/* macros for the EN pin */
#define ClrEN() \
        EN1_ClrVal()                    /* EN=0 */
#define SetEN() \
        EN1_SetVal()                    /* EN=1 */

/* macros for the data bus */
#define DataAsOutput() \
        DB41_SetOutput();               /* set data port as output */ \
        DB51_SetOutput();               /* set data port as output */ \
        DB61_SetOutput();               /* set data port as output */ \
        DB71_SetOutput()                /* set data port as output */
#define DataAsInput() \
        DB41_SetInput();                /* set data port as input */ \
        DB51_SetInput();                /* set data port as input */ \
        DB61_SetInput();                /* set data port as input */ \
        DB71_SetInput()                 /* set data port as input */

/* waiting macros */
#define Waitns(x) \
        WAIT1_Waitns(x)                 /* Wait x ns */
#define Waitus(x) \
        WAIT1_Waitus(x)                 /* Wait x us */
#define Waitms(x) \
        WAIT1_Waitms(x)                 /* Wait x ms */

/* timings from Hitachi HD44708.pdf */
#define Timing_PWeh_ns   230 /* PWeh: Enable Pulse width (high level) */
#define Timing_tAS_ns     40 /* tAB: Address set-up time (RS, RW to E ) */
#define Timing_tDDR_ns   160 /* tDDR: Data delay time */
#define Timing_tCYCLE_ns 500 /* tCYLE: Enable Cycle time */

#define LCDout_WAIT_DISPLAY_US      1000 /* wait time as specified in properties */
#define LCDout_WAIT_LCD_CMD_AFTER_4BIT_DATA1_US     15 /* wait time after first 4bit data */
#define LCDout_WAIT_LCD_CMD_AFTER_4BIT_DATA2_US     60 /* wait time after second 4bit data */

static uint8_t DataGet(void) {
  uint8_t val;

  val =  (DB71_GetVal()<<3)
        |(DB61_GetVal()<<2)
        |(DB51_GetVal()<<1)
        |(DB41_GetVal()<<0);
  return val;
}

static void DataPut(uint8_t val) {
  DB71_PutVal((val&(1<<3))!=0);
  DB61_PutVal((val&(1<<2))!=0);
  DB51_PutVal((val&(1<<1))!=0);
  DB41_PutVal((val&(1<<0))!=0);
}

/* Internal method prototypes */
static void EnablePulse(void);

/*
** ===================================================================
**     Method      :  EnablePulse (component LCDHTA)
**
**     Description :
**         Creates a pulse to transfer data and/or commands
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void EnablePulse(void)
{
  SetEN();
  Waitns(Timing_PWeh_ns);
  ClrEN();                              /* set to 0 to finish pulse */
}


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
void LCDout_WriteLCDCommand(byte cmd)
{
#if LCDout_WAIT_DISPLAY_US > 0
  WAIT1_Waitus(LCDout_WAIT_DISPLAY_US); /* wait for some time not to write to the display while he may be busy with previous command */
#endif
  /* 2 4bit transfer */
  DataPut((byte)((cmd&0xF0)>>4));       /* Write the data (cycle #1) */
  EnablePulse();                        /* transfer data */
  Waitus(LCDout_WAIT_LCD_CMD_AFTER_4BIT_DATA1_US);
  DataPut((byte)(cmd&0x0F) );           /* Write the data (cycle #2) */
  EnablePulse();                        /* do the command transfer */
  Waitus(LCDout_WAIT_LCD_CMD_AFTER_4BIT_DATA2_US);
}

/*
** ===================================================================
**     Method      :  LCDout_Clear (component LCDHTA)
**     Description :
**         Clears the display and moves the cursor to the first line.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void LCDout_Clear(void)
{
  LCDout_WriteLCDCommand(ClearDisplayCmd); /* send the clear command to the LCD */
}

/*
** ===================================================================
**     Method      :  LCDout_Home (component LCDHTA)
**     Description :
**         Moves the cursor to the beginning of the first line.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void LCDout_Home(void)
{
  LCDout_WriteLCDCommand(ReturnHomeCmd); /* moves the cursor to the beginning of the first line */
}

/*
** ===================================================================
**     Method      :  LCDout_WriteLCDData (component LCDHTA)
**
**     Description :
**         Writes data to the display
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void LCDout_WriteLCDData(byte ch)
{
#if LCDout_WAIT_DISPLAY_US > 0
   WAIT1_Waitus(LCDout_WAIT_DISPLAY_US); /* wait for some time not to write to the display while he may be busy with previous command */
#endif
   SetRS();                             /* RS = 1: data mode */
   /* 2 4bit transfer */
   DataPut((byte)((ch&0xF0)>>4));       /* Write the data (cycle #1) */
   EnablePulse();                       /* transfer data */
   Waitus(LCDout_WAIT_LCD_CMD_AFTER_4BIT_DATA1_US);
   DataPut((byte)(ch&0x0F));            /* Write the data (cycle #2) */
   EnablePulse();                       /* do the command transfer */
   Waitus(LCDout_WAIT_LCD_CMD_AFTER_4BIT_DATA2_US);
   ClrRS();                             /* RS = 0: back to command mode */
}

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
void LCDout_GotoXY(byte line, byte column)
{
/* row is in the range 1..LCD_Nof_Lines and column in the range 1..LCD_Nof_Columns */
  LCDout_WriteLCDCommand((byte)(GotoXYCmd + (column-1) + (line==1 ? 0x00:Line2Offset)));
}

/*
** ===================================================================
**     Method      :  LCDout_ShiftLeft (component LCDHTA)
**     Description :
**         Shifts all characters to the left.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void LCDout_ShiftLeft(void)
{
  LCDout_WriteLCDCommand(LeftShiftCmd);
}

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
/* The method is implemented as a macro, see LCDout.h */
/*
** ===================================================================
**     Method      :  LCDout_WriteLn (component LCDHTA)
**     Description :
**         Writes a new line to the display
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void LCDout_WriteLn(void)
{
  LCDout_WriteLCDCommand(SecondLineCmd);
}

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
void LCDout_WriteString(char *str)
{
  while (*str != '\0'){
    LCDout_Write(*str);
    str++;
  }
}

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
void LCDout_WriteLineStr(byte line, char *str)
{
  byte i;

  LCDout_Line(line);                    /* select line */
  for(i = 0; i < LCDout_MAX_LCD_LINE_CHARS && *str!='\0'; i++, str++) {
    LCDout_Write(*str);
  }
  /* Clear the rest of the line */
  for (; i < LCDout_MAX_LCD_LINE_CHARS; i++) {
    LCDout_Write(' ');
  }
}

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
void LCDout_Line(byte line)
{
  LCDout_WriteLCDCommand((byte)(line==1?FirstLineCmd:SecondLineCmd));
}

/*
** ===================================================================
**     Method      :  LCDout_Init (component LCDHTA)
**
**     Description :
**         Display driver initialization routine.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void LCDout_Init(void)
{
  /* This function initializes the driver.
     The low level init already shall have set our data port to input/output, currently set to output,
     plus all control pins are set as outputs with low values. To be sure, we do it here again. */
  ClrEN();                              /* EN Pin low */
  ClrRS();                              /* RS Pin low: command mode */
  DataPut(0);

  /* make pins output */
  DataAsOutput();

  /* send the reset sequence according to the data sheet */
  Waitms(80);                           /* wait for more than 15ms after Vcc rises to 4.5V, wait for more than 40ms after Vcc rises to 2.7V. In case of POR (Power On Reset) we need some additional time. */
  DataPut(0x3);                         /* BF cannot be checked before this function, function set (interface is 8bits long) */
  EnablePulse();                        /* transfer data */
  Waitms(5);                            /* wait for more than 4.1 ms */
  /* 0x3 is already on the bus from previous DataPut(), do not need to put it again here */
  EnablePulse();                        /* transfer data */
  Waitus(100);                          /* wait for more than 100us */
  /* 0x3 is already on the bus from previous DataPut(), do not need to put it again here */
  EnablePulse();                        /* transfer data */
  Waitus(100);                          /* wait for more than 100us */

  DataPut(0x2);                         /* Function set */
  EnablePulse();                        /* transfer data */
  Waitus(100);

  LCDout_WriteLCDCommand(FunctionSetCmd|FunctionSet_Font5x8
                 |FunctionSet_4bit      /* we are using 4bit data bus */ \
                 |FunctionSet_2Lines    /* we are using two or more lines */
                 );

  LCDout_WriteLCDCommand(DisplayOnOffControlCmd|DisplayOnOffControl_DisplayOn);
  LCDout_WriteLCDCommand(ClearDisplayCmd); /* Clear display */
  LCDout_WriteLCDCommand(EntryModeSetCmd|EntryModeSet_IncrementOn); /* Entry mode set: Increment mode, display shift off */
}

/*
** ===================================================================
**     Method      :  LCDout_ShiftRight (component LCDHTA)
**     Description :
**         Shifts all characters to the right.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void LCDout_ShiftRight(void)
{
  LCDout_WriteLCDCommand(RightShiftCmd);
}

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
void LCDout_SetEntryMode(bool increment, bool shiftLeft)
{
  byte flags = 0;

  if (increment) {
    flags |= EntryModeSet_IncrementOn;
  }
  if (shiftLeft) {
    flags |= EntryModeSet_ShiftOn;
  }
  LCDout_WriteLCDCommand((byte)(EntryModeSetCmd|flags)); /* Entry mode set: Increment mode, display shift mode */
}

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
byte LCDout_UseDisplay(byte display)
{
  (void)display; /* not used, as not using E2 (additional enable) signal */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  LCDout_CursorShiftRight (component LCDHTA)
**     Description :
**         Shift the cursor to the right.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void LCDout_CursorShiftRight(void)
{
  LCDout_WriteLCDCommand(RightShiftCursor);
}

/*
** ===================================================================
**     Method      :  LCDout_CursorShiftLeft (component LCDHTA)
**     Description :
**         Shift the cursor to the left.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void LCDout_CursorShiftLeft(void)
{
  LCDout_WriteLCDCommand(LeftShiftCursor);
}

/* END LCDout. */

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
