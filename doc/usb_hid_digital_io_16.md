
The Digital I/O device details:
=========================

* Electrical characteristics: see the original vendor documentation.
* There are 16 I/O pins numbered 0-15. Each pin can be set up as input or output. An input pin can be connected to internal pull-up or stay floating.
* The board can be connected to external 5V source to keep the configured state when disconnected from host, or while the host is shut down.
* The board has one green LED which can be used for any purpose; it is not associated with any of I/O pins.
- The USB interface is low-speed (USB 1.1) HID, based on [V-USB](http://www.obdev.at/products/vusb/) software USB emulation library.
- The USB device ID is VEN=16C0 DEV=05DF. This ID is shared by variety of V-USB HID compatible devices. The V-USB documentation describes how to detect the real vendor and product IDs for such devices.
- For this device, the vendor ID is **TBD** and product ID is "USBIO" **TBD**.
- Each board has an unique 4-character ID string in its HID descriptor (not to be confused with the standard USB "serial number"). We don't know whether these IDs are really unique, at least, on all our devices they are different.
* The host must poll to detect change of inputs. The device does not have events or interrupts.
