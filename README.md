# g29emu v0.1

First of all, big thanks to matlo for creating [GIMX](https://github.com/matlo/GIMX). This is the base of g29emu.

After playing around with [GIMX](https://github.com/matlo/GIMX), it works perfectly for my Logitech Driving Force GT (G29 emulated) for F1 2018 on my PS4. However, it was not very portable to my home TV. I thought a Raspberry Pi would solve the problem, but too many libusb timeouts caused my forcefeedback to glitch as e.g. force did not reset to normal.

Therefore i thought of a stripped down version of GIMX that does about the same job in a single microcontroller (currently ATMega32U4). It is way less configurable yet and device support is only Driving Force GT -> G29 PS4 at the moment.

# Ingredients:

- Arduino atmega32u4 board (pro micro/leonardo) 16MHz/5V
- USB Host shield (or another board having a MAX3421E USB to SPI chip)
- Arduino one of the latest version (i am using 1.8.5)
- LUFA for Arduino (https://github.com/Palatis/Arduino-Lufa)
- USB Host shield library by Oleg Mazurov (Circuits@Home)
- GIMX Firmware sources from matlo (https://github.com/matlo/GIMX-firmwares)
- USB HUB (you need two ports, one for the wheel and another for the PS4 controller)

I cloned the G29EMUPS4 files, stripped the UART commands and implemented the USB Host function. I also had to rewrite a Serial class for debugging, as the Arduino stock serial class did not work for me using the LUFA library (it replaces some USB stacks in Arduino, probably also the HW serial port)

# Currently working:
- Wheel is working on PS4, most buttons and pedals working
- Authenticate trough genuine controller
- PS4 Detects G29 wheel (Active with PS button)
- Driving Force GT wheel initiation working (exit compatible mode, enter Force GT mode)
- USB Hub support
- Mapping the buttons from DFGT to G29 report
- Forcefeedback working (thanks to the Logitech_Force_Feedback_Protocol_V1.5.pdf)

# TODO:

- Latency check / Improving latency
- Missing packet check. Make sure all packets arrive at DFGT.
- All buttons working. Missing buttons are: Plus,Minus,Handshift up, Handshift Down, GT Button, red dial, Enter button
- Allow other Logitech Wheels for input
- Add G27/G29 style RPM LED support (need extra hardware on wheel with LEDs)

# Install:

Clone the files, Install Arduino, install the LUFA and USB Host Shield library stated above.
When compiling, the compile raises an error ('Serial not defined') in the USB Host library.
This needs an ugly hack that needs a proper solution

Open 'Arduino\libraries\USB_Host_Shield_Library_2.0\settings.h'

change this:
```c
#ifndef USB_HOST_SERIAL
#define USB_HOST_SERIAL Serial
#endif
```

with

```c
#ifndef USB_HOST_SERIAL
class LUFASerial {
	private:
	public:
	 LUFASerial();
   void begin(long baud);
	 void print(const char* StringPtr);
   void print(long value);
   void write(int value);
   void printhex(int value);
   void println(long value,int type);
	 void println(const char* StringPtr);
   void println(long value);
};

extern LUFASerial Serial;


#define USB_HOST_SERIAL Serial
#endif
```

