
The DCT "USB I/O CARD" device details:
=========================

* Electrical characteristics: see the vendor documentation.
* The board has 16 pins numbered 0-15. Each pin can be set up as input or output. Any input pin can be connected to internal pull-up or stay floating.
* The host must poll to detect change of inputs. The device does not send reports by itself when inputs state change.
* The board can be connected to external 5V source to keep the configured state while disconnected from the host, or when the host shuts down.
* The board has one green blinking LED which can be used for any purpose. For example, to test that the device is alive.
- The USB interface is low-speed (USB 1.1) HID, based on [V-USB](http://www.obdev.at/products/vusb/) software USB emulation library.
- The USB device ID is VEN=16C0 DEV=05DF. This ID is shared by variety of V-USB HID compatible devices. The V-USB documentation describes how to detect the real vendor and product IDs for such devices.
- For this device, the real vendor ID is "www.dcttech.com" and product ID is "USBIO".
- Each board has an unique 4-character ID string (not to be confused with the standard USB "serial number"). 
