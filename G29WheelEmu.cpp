/*
  Copyright 2019  Mathijs van den Berg (mathijsvandenberg3 [at] gmail [dot] com)
  Redistributed under the GPLv3 licence.
  Based on code by
    Mathieu Laurendeau (mat.lau [at] laposte [dot] net)
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


#include "G29WheelEmu.h"
#include "DrivingForceGT.h"
#include "PS4AuthDevice.h"

extern DrivingForceGT DFGT;
extern PS4AuthDevice PS4AD;

#define REPORT_TYPE_FEATURE 3
#define MAX_CONTROL_TRANSFER_SIZE 64

extern uint8_t DFGT_rep[64];
extern LUFASerial Serial;

static uint8_t report[] = {
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

const char PROGMEM buff3[] = {
  0xF3, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00
};

const char PROGMEM buf03[] =
{
  0x03, 0x21, 0x27, 0x03, 0x11, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0D, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x84, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

volatile unsigned char sendReport = 0;
volatile unsigned char started = 0;
static unsigned char ready = 0;


/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

  GlobalInterruptEnable(); // enable global interrupts
 
}

void InitUSB()
{
    /* Hardware Initialization */
  USB_Init();
  started = 1;
}

/** Sends the next HID report to the host, via the IN endpoint. */
void SendNextReport(void)
{
  /* Select the IN Report Endpoint */
  Endpoint_SelectEndpoint(IN_EPNUM);

  if (ready && sendReport)
  {
    /* Wait until the host is ready to accept another packet */
    while (!Endpoint_IsINReady()) {}

    /* Write IN Report Data */

    memcpy(report,DFGT_rep,49);
    if (report[7] == 1)
    {
      Serial.print("PS = 1");
    }

    Endpoint_Write_Stream_LE(report, sizeof(report), NULL);
    
    sendReport = 0;

    /* Finalize the stream transfer to send the last packet */
    Endpoint_ClearIN();
  }
}

/** Reads the next OUT report from the host from the OUT endpoint, if one has been sent. */
void ReceiveNextReport(void)
{
  unsigned char buffer[EPSIZE];
  uint16_t length = 0;
  uint16_t x;
  
  /* Select the OUT Report Endpoint */
  Endpoint_SelectEndpoint(OUT_EPNUM);

  /* Check if OUT Endpoint contains a packet */
  if (Endpoint_IsOUTReceived())
  {
    /* Check to see if the packet contains data */
    if (Endpoint_IsReadWriteAllowed())
    {
      /* Read OUT Report Data */
      uint8_t ErrorCode = Endpoint_Read_Stream_LE(buffer, sizeof(buffer), &length);
      if(ErrorCode == ENDPOINT_RWSTREAM_NoError)
      {
        length = sizeof(buffer);
      }
    }

    /* Handshake the OUT Endpoint - clear endpoint and ready for next report */
    Endpoint_ClearOUT();

    if(length)
    {
      
      switch (buffer[1])
      {
        case 0xF8:
          if (buffer[2] == 1) { Serial.println("G29: Change Mode to Driving Force Pro");}
          if (buffer[2] == 2) { Serial.println("G29: Change Wheel Range to 200 degrees");}
          if (buffer[2] == 3) { Serial.println("G29: Change Wheel Range to 900 degrees");}
          if (buffer[2] == 9) { Serial.println("G29: Change Device Mode");}
          if (buffer[2] == 0x12) {  } // LEDS
          if (buffer[2] == 0x81)
          { 
            Serial.print("G29: Wheel Range Change: ");
            x = buffer[3] + (buffer[4]<<8);
            Serial.print(x);
            Serial.println(" Degrees");
          }
          break;
        default:
          break;
          Serial.print("G29: Command 0x");
          Serial.printhex(buffer[1]);
          Serial.print(" [");

          for (x=0;x<length;x++)
          {
            Serial.printhex(buffer[x]);
            Serial.print(" ");
          }
          Serial.println("]");
          break;
      }
      DFGT.SendFFB(buffer);
    }
  }
}


/** Function to manage HID report generation and transmission to the host, when in report mode. */
void HID_Task(void)
{
  /* Device must be connected and configured for the task to run */
  if (USB_DeviceState != DEVICE_STATE_Configured)
    return;

  /* Send the next keypress report to the host */
  SendNextReport();
    
  /* Process the LED report sent from the host */
  ReceiveNextReport();
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{

}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
  Serial.println("USB: Device Connected");
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
   Serial.println("USB: Device Disconnected");
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
   
	if (!(Endpoint_ConfigureEndpoint(IN_EPNUM, EP_TYPE_INTERRUPT, EPSIZE, 1)))
  {
    //LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
  }
  
  if (!(Endpoint_ConfigureEndpoint(OUT_EPNUM, EP_TYPE_INTERRUPT, EPSIZE, 1)))
  {
    //LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
  }
  Serial.println("USB: Configuration Changed");
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
  static unsigned char buffer[MAX_CONTROL_TRANSFER_SIZE];

  /* Handle HID Class specific requests */
  switch (USB_ControlRequest.bRequest)
  {
    case REQ_GetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        uint8_t reportType = USB_ControlRequest.wValue >> 8;
        uint8_t reportId = USB_ControlRequest.wValue & 0xff;

        if(reportType == REPORT_TYPE_FEATURE)
        {
          const void* feature = NULL;
          unsigned char len = 0;

          switch(reportId)
          {
            case 0xf1:
              len = PS4AD.GetReport(buffer,0xF1);
              Endpoint_ClearSETUP();
              Endpoint_Write_Control_Stream_LE(buffer, len);
              Endpoint_ClearOUT();
              Serial.println("GetReport request F1");
              break;
            case 0xf2:
              len = PS4AD.GetReport(buffer,0xF2);
              Endpoint_ClearSETUP();
              Endpoint_Write_Control_Stream_LE(buffer, len);
              Endpoint_ClearOUT();
              Serial.println("GetReport request F2");
              break;
            case 0x03:
              Serial.println("GetReport request 03");
              feature = buf03;
              len = sizeof(buf03);
              ready = 1;
              break;
            case 0xf3:
              Serial.println("GetReport request F3");
              feature = buff3;
              len = sizeof(buff3);
              break;
            default:
              Serial.println("GetReport request (default) ");
              Serial.println(reportId);
              break;
          }

          if(feature)
          {
            Endpoint_ClearSETUP();
            Endpoint_Write_Control_PStream_LE(feature, len);
            Endpoint_ClearOUT();
          }
        }
      }
    
      break;
    case REQ_SetReport:
      if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
      {
        Endpoint_ClearSETUP();
        Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
        Endpoint_ClearIN();

        uint8_t reportType = USB_ControlRequest.wValue >> 8;
        uint8_t reportId = USB_ControlRequest.wValue & 0xff;

        if(reportType == REPORT_TYPE_FEATURE)
        {
          switch(reportId)
          {
            case 0xf0:
              Serial.println("SetReport request F0 (auth)");
              PS4AD.SetReport(buffer,0xF0);
               break;
            default:
              Serial.println("SetReport request (unknown)");
              Serial.println(reportId);
              break;
          }
        }
      }
      break;
  }
}

