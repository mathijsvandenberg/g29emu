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

#include "PS4AuthDevice.h"

uint8_t report_f0[64];

void PS4AuthDevice::ParseHIDData(USBHID *hid __attribute__((unused)), bool is_rpt_id __attribute__((unused)), uint8_t len, uint8_t *buf)
{
  static int firstdata=1;
  if (HIDUniversal::VID == PS4AuthDevice_VID && (HIDUniversal::PID == PS4AuthDevice_PID || HIDUniversal::PID == PS4AuthDevice_PID2) && len > 2 && buf)
  {
    if (firstdata==1)
    {
      Serial.println("Get data from PS4AuthDevice");
      firstdata=0;
    }
  }
};

uint8_t PS4AuthDevice::OnInitSuccessful()
{
  if (HIDUniversal::VID == PS4AuthDevice_VID && (HIDUniversal::PID == PS4AuthDevice_PID || HIDUniversal::PID == PS4AuthDevice_PID2))
  {
    Serial.println("Init succesfull PS4AuthDevice");
    if (pFuncOnInit)
    {
      
      pFuncOnInit(); // Call the user function
    }
  };
  return 0;
};

int PS4AuthDevice::GetReport(unsigned char * buffer,int report)
{
  int len=0;
  int x;
  if (report == 0xF1)
  {
    Serial.print("[PS4AuthDevice GetReport] Got 0xF1 report request ");
    pUsb->ctrlReq(bAddress, epInfo[0].epAddr, bmREQ_HID_IN, HID_REQUEST_GET_REPORT, 0xf1, 0x03, 0x00, 64, 64, buffer, NULL);
    Serial.print("SEQ=");
    Serial.printhex(buffer[1]);
    Serial.print(" REP=");
    Serial.printhex(buffer[2]);
    Serial.println("");
    len=64;
  }
  
  if (report == 0xF2)
  {
    pUsb->ctrlReq(bAddress, epInfo[0].epAddr, bmREQ_HID_IN, HID_REQUEST_GET_REPORT, 0xf2, 0x03, 0x00, 16, 16, buffer, NULL);
    Serial.print("Got F2 reply: ");
    for (x=0;x<16;x++)
    {
      Serial.printhex(buffer[x]);
      Serial.print(" ");
    }
    Serial.println("");
    len=16;
  }
  return len;
  
}

void PS4AuthDevice::SetReport(unsigned char * buffer,int report)
{
  int x;
  if (report == 0xF0)
  {
    memcpy(report_f0,buffer,64);    
    pUsb->ctrlReq(bAddress, epInfo[0].epAddr, bmREQ_HID_OUT, HID_REQUEST_SET_REPORT, 0xf0, 0x03, 0x00, 64, 64, buffer, NULL);
    Serial.print("[PS4AuthDevice SetReport] Got 0xF0 report SEQ=");
    Serial.printhex(report_f0[1]);
    Serial.print(" REP=");
    Serial.printhex(report_f0[2]);
    for (x=0;x<64;x++)
    {
      if ((x % 16) == 0) { Serial.println(""); }
      Serial.printhex(report_f0[x]);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

