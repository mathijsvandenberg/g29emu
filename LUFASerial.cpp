/*
  Copyright 2019  Mathijs van den Berg (mathijsvandenberg3 [at] gmail [dot] com)
  Redistributed under the GPLv3 licence.
  Original licence:
  Permission to use, copy, modify, distribute, and sell this 
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in 
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting 
  documentation, and that the name of the author not be used in 
  advertising or publicity pertaining to distribution of the 
  software without specific, written prior permission.
  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.

  NOTE: Sloppy code, just needed an arduino like Serial class because the USB Shield library needed one!
  
*/


// LUFASerial, because the arduino default serial doesnt work!


#include <LUFA/LUFA/Drivers/Peripheral/Serial.h>
#include "LUFASerial.h"

LUFASerial::LUFASerial()
{

}

void LUFASerial::begin(long baud)
{
 Serial_Init(baud, false);
}
 
void LUFASerial::print(const char* StringPtr)
{
 uint8_t CurrByte;

  while ((CurrByte = *StringPtr) != 0x00)
  {
    Serial_SendByte(CurrByte);
    StringPtr++;
  }
}

void LUFASerial::print(long value)
{
  int a;
  byte temp[16];
  for (a=0;a<16;a++)
  {
    temp[a]=0x00;
  }
  a=0;
  
  while(value>0 && a < 16)
  {
    temp[a++] = (value % 10);
    value=value/10;
  }

  if (a==0) { Serial_SendByte(0x30); return; }
  for (a=15;a>0;a--)
  {
    if (temp[a] > 0) { Serial_SendByte(0x30 + temp[a]); }
  }
  Serial_SendByte(0x30 + temp[0]);
}

void LUFASerial::printhex(int value)
{
  int v1,v2;

  v1 = (value >> 4) & 0x0F;
  switch(v1)
  {
    case 0:  Serial_SendByte('0');break;
    case 1:  Serial_SendByte('1');break;
    case 2:  Serial_SendByte('2');break;
    case 3:  Serial_SendByte('3');break;
    case 4:  Serial_SendByte('4');break;
    case 5:  Serial_SendByte('5');break;
    case 6:  Serial_SendByte('6');break;
    case 7:  Serial_SendByte('7');break;
    case 8:  Serial_SendByte('8');break;
    case 9:  Serial_SendByte('9');break;
    case 10:  Serial_SendByte('A');break;
    case 11:  Serial_SendByte('B');break;
    case 12:  Serial_SendByte('C');break;
    case 13:  Serial_SendByte('D');break;
    case 14:  Serial_SendByte('E');break;
    case 15:  Serial_SendByte('F');break;
    default: break;
  }
  v2 = value & 0x0F;
  switch(v2)
  {
    case 0:  Serial_SendByte('0');break;
    case 1:  Serial_SendByte('1');break;
    case 2:  Serial_SendByte('2');break;
    case 3:  Serial_SendByte('3');break;
    case 4:  Serial_SendByte('4');break;
    case 5:  Serial_SendByte('5');break;
    case 6:  Serial_SendByte('6');break;
    case 7:  Serial_SendByte('7');break;
    case 8:  Serial_SendByte('8');break;
    case 9:  Serial_SendByte('9');break;
    case 10:  Serial_SendByte('A');break;
    case 11:  Serial_SendByte('B');break;
    case 12:  Serial_SendByte('C');break;
    case 13:  Serial_SendByte('D');break;
    case 14:  Serial_SendByte('E');break;
    case 15:  Serial_SendByte('F');break;
    default: break;
  }
}




void LUFASerial::println(const char* StringPtr)
{
  LUFASerial::print(StringPtr);
  Serial_SendByte('\r');
  Serial_SendByte('\n');
 return;
}

void LUFASerial::write(int value)
{
  Serial_SendByte(value);
 return;
}

void LUFASerial::println(long value)
{
  LUFASerial::print(value);
  Serial_SendByte('\r');
  Serial_SendByte('\n');
 return;
}

void LUFASerial::println(long value,int type)
{
  LUFASerial::print(value);
  Serial_SendByte('\r');
  Serial_SendByte('\n');
 return;
}
