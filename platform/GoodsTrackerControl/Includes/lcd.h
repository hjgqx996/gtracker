/*
 * lcd.h
 *
 *  Created on: 19/06/2017
 *      Author: Fl�vio Soares
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t byte;

/*
 *
 */
void LCDInit();

/*
 *
 */
void LCDWrite(char ch);

/*
 *
 */
void LCDWriteLn(void);

/*
 *
 */
void LCDWriteLineStr(byte line, char *str);

/*
 *
 */
void LCDWriteString(char *str);

/*
 *
 */
void LCDShiftLeft(void);

/*
 *
 */
void LCDShiftRight(void);

/*
 *
 */
void LCDGotoXY(byte line, byte column);

/*
 *
 */
void LCDSetEntryMode(bool increment, bool shiftLeft);

/*
 *
 */
void LCDCursorShiftRight(void);

/*
 *
 */
void LCDCursorShiftLeft(void);

/*
 *
 */
void LCDHome(void);

/*
 *
 */
void LCDLine(byte line);

/*
 *
 */
void LCDClear(void);


#endif /* LCD_H_ */
