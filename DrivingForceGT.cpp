/*
  Copyright 2019  Mathijs van den Berg (mathijsvandenberg3 [at] gmail [dot] com)
  Redistributed under the GPLv3 licence.
  Based on code by
    Mathieu Laurendeau (mat.lau [at] laposte [dot] net)
    Circuits At Home, LTD (support [at] circuitsathome.com)
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

#include "DrivingForceGT.h"

uint8_t DFGT_rep[] = {
    0x01, //Report ID
    0x80, 0x80, 0x80, 0x80, //X, Y, Z, Rz (unused)
    0x08, //4 MSB = 4 buttons, 4 LSB = hat switch
    0x00, 0x00, //10 buttons
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x80, //wheel
    0xff, 0xff, //gas pedal
    0xff, 0xff, //break pedal
    0xff, 0xff, //unknown
    0x00,
    0xff, 0xff, //unknown
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00
};

int wheel=0;

extern volatile unsigned char sendReport;

void DrivingForceGT::ParseHIDData(USBHID *hid __attribute__((unused)), bool is_rpt_id __attribute__((unused)), uint8_t len, uint8_t *buf)
{
  if (HIDUniversal::VID == DrivingForceGT_VID && (HIDUniversal::PID == DrivingForceGT_Startup_PID) && len > 2 && buf)
  {
        uint8_t buf[7]; // Do not revert identity
        buf[0] = 0xF8;
        buf[1] = 0x0A;
        buf[2] = 0x00;
        buf[3] = 0x00;
        buf[4] = 0x00;
        buf[5] = 0x00;
        buf[6] = 0x00;
        DrivingForceGT_Command(buf, sizeof(buf));

        buf[0] = 0xF8; 
        buf[1] = 0x09; // Change device mode 
        buf[2] = 0x03; // Logitech Driving Force GT
        buf[3] = 0x01; // 1=Switch identity after detaching from USB
        buf[4] = 0x00;
        buf[5] = 0x00;
        buf[6] = 0x00;
        DrivingForceGT_Command(buf, sizeof(buf));  
  }

  
  if (HIDUniversal::VID == DrivingForceGT_VID && (HIDUniversal::PID == DrivingForceGT_Running_PID) && len > 2 && buf)
  {
    /*
    int x;
    Serial.print("DFGT Running GET DATA: [");
    for (x=0;x<len;x++)
    {
      Serial.print(buf[x]);
      Serial.print(" ");
    }
    
    Serial.println("]");
    */

    // Parsing DFGT to G29 data
    DFGT_rep[5] = (buf[0] & 0xCF) + ((buf[0] & 0x10) << 1) + ((buf[0] & 0x20) >> 1); // Change X and square


    // Buttons are mapped a bit odd on the DFGT, so there is some shifting needed to align buttons.
    DFGT_rep[6] = ((buf[1] & 0x01) << 1) + \
                  ((buf[1] & 0x02) >> 1) + \
                  ((buf[1] & 0x04) << 1) + \
                  ((buf[1] & 0x08) >> 1) + \
                  ((buf[1] & 0x10) << 0) + \
                  ((buf[1] & 0x20) >> 0) + \
                  ((buf[1] & 0x40) << 1) + \
                  ((buf[1] & 0x80) >> 1);
                  
    DFGT_rep[7] = buf[3] & 0x01; // PS button

    DFGT_rep[45] = buf[6]; // GAS
    DFGT_rep[46] = buf[6]; // GAS

    DFGT_rep[47] = buf[7]; // Brake
    DFGT_rep[48] = buf[7]; // Brake

    wheel = buf[4] + buf[5]*256;
    wheel = wheel * 4;
 
    DFGT_rep[43] = wheel % 256; // Wheel
    DFGT_rep[44] = wheel / 256; // Wheel

    // Did i say all buttons work?? Well most of them
    // TODO: ALL buttons work
    
    sendReport=1;
    
  }
  
};

uint8_t DrivingForceGT::OnInitSuccessful()
{
  if (HIDUniversal::VID == DrivingForceGT_VID && (HIDUniversal::PID == DrivingForceGT_Startup_PID || HIDUniversal::PID == DrivingForceGT_Running_PID))
  {
    Serial.println("Init succesfull DrivingForceGT");
    if (pFuncOnInit)
    {
      pFuncOnInit(); // Call the user function
    }
  };
  return 0;
};

void DrivingForceGT::SendFFB(unsigned char * buffer)
{
  DrivingForceGT_Command(buffer+1, 7);
}

void DrivingForceGT::DrivingForceGT_Command(uint8_t *data, uint16_t nbytes)
{
  pUsb->outTransfer(bAddress,0x01, nbytes, data);
};
