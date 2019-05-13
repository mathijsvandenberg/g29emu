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

#ifndef _PS4AuthDevice_h_
#define _PS4AuthDevice_h_

#include "hiduniversal.h"

#define HORIPAD_VID     0x0F0D
#define HORIPAD_PID     0x0055

#define PS4_VID         0x054C // Sony Corporation
#define PS4_PID         0x05C4 // PS4 Controller
#define PS4_PID_SLIM    0x09CC // PS4 Slim Controller


#define PS4AuthDevice_VID PS4_VID // Logitech
#define PS4AuthDevice_PID PS4_PID // Driving Force GT
#define PS4AuthDevice_PID2 PS4_PID_SLIM // Driving Force GT

/**
 * This class implements support for the authentication of a PS4 controller.
 * It uses the HIDUniversal class for all the USB communication.
 */
class PS4AuthDevice : public HIDUniversal {
public:
        /**
         * Constructor for the PS4AuthDevice class.
         * @param  p   Pointer to the USB class instance.
         */
        PS4AuthDevice(USB *p) :
        HIDUniversal(p) 
        {

        };

        /**
         * Used to check if a PS4 controller is connected.
         * @return Returns true if it is connected.
         */
        bool connected() {
                return HIDUniversal::isReady() && HIDUniversal::VID == PS4AuthDevice_VID && (HIDUniversal::PID == PS4AuthDevice_PID || HIDUniversal::PID == PS4AuthDevice_PID2);
        };

         int GetReport(unsigned char * buffer,int report);
         void SetReport(unsigned char * buffer,int report);

        /**
         * Used to call your own function when the device is successfully initialized.
         * @param funcOnInit Function to call.
         */
        void attachOnInit(void (*funcOnInit)(void)) {
                pFuncOnInit = funcOnInit;
        };

protected:
        /** @name HIDUniversal implementation */
        /**
         * Used to parse USB HID data.
         * @param hid       Pointer to the HID class.
         * @param is_rpt_id Only used for Hubs.
         * @param len       The length of the incoming data.
         * @param buf       Pointer to the data buffer.
         */
        void ParseHIDData(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);

        /**
         * Called when a device is successfully initialized.
         * Use attachOnInit(void (*funcOnInit)(void)) to call your own function.
         * This is useful for instance if you want to set the LEDs in a specific way.
         */
        uint8_t OnInitSuccessful();
        /**@}*/

        /** @name USBDeviceConfig implementation */
        /**
         * Used by the USB core to check what this driver support.
         * @param  vid The device's VID.
         * @param  pid The device's PID.
         * @return     Returns true if the device's VID and PID matches this driver.
         */
        virtual bool VIDPIDOK(uint16_t vid, uint16_t pid) {
                return (vid == PS4AuthDevice_VID && (pid == PS4AuthDevice_PID || pid == PS4AuthDevice_PID2));
        };
        /**@}*/

private:
        void (*pFuncOnInit)(void); // Pointer to function called in onInit()
};
#endif
