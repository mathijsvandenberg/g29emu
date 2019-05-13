/*
  Copyright 2019  Mathijs van den Berg (mathijsvandenberg3 [at] gmail [dot] com)
  Redistributed under the GPLv3 licence.
  Based on code by
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

#ifndef _DrivingForceGT_h_
#define _DrivingForceGT_h_

#include "hiduniversal.h"

#define DrivingForceGT_VID 0x046D // Logitech
#define DrivingForceGT_Startup_PID 0xC294 // Driving Force GT compatible mode
#define DrivingForceGT_Running_PID 0xC29A // Driving Force GT


/**
 * This class implements support for the PS Buzz controllers via USB.
 * It uses the HIDUniversal class for all the USB communication.
 */
class DrivingForceGT : public HIDUniversal {
public:
        /**
         * Constructor for the DrivingForceGT class.
         * @param  p   Pointer to the USB class instance.
         */
        DrivingForceGT(USB *p) :
        HIDUniversal(p) 
        {

        };

        /**
         * Used to check if a PS Buzz controller is connected.
         * @return Returns true if it is connected.
         */
        bool connected() {
                return HIDUniversal::isReady() && HIDUniversal::VID == DrivingForceGT_VID && (HIDUniversal::PID == DrivingForceGT_Startup_PID || HIDUniversal::PID == DrivingForceGT_Running_PID);
        };


       void SendFFB(unsigned char * buffer);
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
                return (vid == DrivingForceGT_VID && pid == (DrivingForceGT_Startup_PID || DrivingForceGT_Running_PID));
        };
        /**@}*/

private:
        void (*pFuncOnInit)(void); // Pointer to function called in onInit()
        void DrivingForceGT_Command(uint8_t *data, uint16_t nbytes);

};
#endif
