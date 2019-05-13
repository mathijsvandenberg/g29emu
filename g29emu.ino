/*
  Copyright 2019  Mathijs van den Berg (mathijsvandenberg3 [at] gmail [dot] com)
  Redistributed under the GPLv3 licence.
  Based on code by
    Mathieu Laurendeau (mat.lau [at] laposte [dot] net)
    Denver Gingerich (denver [at] ossguy [dot] com)
    Dean Camera (dean [at] fourwalledcubicle [dot] com)
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
*/

/** \file
 *
 *  Main source file for the controller. This file contains the main tasks and
 *  is responsible for the initial application hardware configuration.
 */

#include "LUFAConfig.h"
#include <LUFA.h>
#include "G29WheelEmu.h"
#include "DrivingForceGT.h"
#include "PS4AuthDevice.h"
#include <usbhub.h>
#include <SPI.h>



// USB Host side. Depending on the USB HUB used, more or less USBHub declarations are needed
USB Usb;
USBHub  Hub1(&Usb);

// The input device
DrivingForceGT DFGT(&Usb);

// The authentication device
PS4AuthDevice PS4AD(&Usb);

void setup()
{
	SetupHardware(); // ask LUFA to setup the hardware
  Serial.begin(115200);
  Serial.println("g29emu version 0.1 by Mathijs van den Berg");
  Serial.println("Based on GIMX by Mathieu Laurendeau");
  Serial.println("Input device: Logitech Driving Force GT");
  Serial.println("Output device: Logitech G29 PS4");
  
  if (Usb.Init() == -1)
  {
      Serial.println("OSC did not start.");
  }
  delay(200);
  InitUSB();
}

void loop()
{
 	HID_Task();
	USB_USBTask();
  Usb.Task();
}

