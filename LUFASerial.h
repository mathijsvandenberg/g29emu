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

#ifndef LUFASerial_h
#define LUFASerial_h


#include "Arduino.h"

class LUFASerial {
	private:
	public:
	 LUFASerial();
   void begin(long baud);
	 void print(const char* StringPtr);
   void print(long value);
   void printhex(int value);
   void write(int value);
	 void println(const char* StringPtr);
   void println(long value);
   void println(long value,int type);
};

#endif
