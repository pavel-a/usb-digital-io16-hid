How to use `usb_io_interface` library
===================================

Version 2.0

Changes to the original DLL:
----------------------------

* The type used for handles changed from int to intptr_t, for 64-bit compatibility. This should be binary compatible with existing 32-bit clients.
* Added helper functions for use from managed languages and scripts; see below.
 TODO

Windows, Visual C++ applications
--------------------------------


Include file name: `usb_io_device.h`  
Link library file name: `usb_io_interface.lib`  
Dynamic library file name: `usb_io_interface.dll`

Put the following lines in your source file:

    #include <usb_relay_device.h>
    #pragma comment(lib, "usb_io_interface")


The file `usb_io_interface.dll` must be installed with your application. Use either 32-bit or 64-bit DLL, matching your application. The VC++ redistributable runtime library may be required if the library is build with this option.


Functions
-------------

* `usb_io_init` - Call this before calling other library functions
* `usb_io_get_device_list` - Enumerates all digital i/o devices plugged into the PC. Returns list of `usb_io_device_info` structures. Caller should free this list after finishing using the devices, by passing it to `usb_io_free_device_list`.
* `usb_io_free_device_list`- frees the list returned by `usb_io_get_device_list`
* `usb_io_open_device` - opens a digital i/o device
* `usb_io_get_all_pin_info` - returns array of `pin_info` structures for each pin
* `usb_io_read_input_pin_value` - read state of an input pin
* `usb_io_set_pin_mode` - configure one I/O pin as input or output
* `usb_io_write_output_pin_value` - set state of an output oin
* `usb_io_close_device` - closes the device handle opened by `usb_io_open_device`
* `usb_io_set_work_led_mode` - turn the LED on or off
* `usb_io_uninit` -  Finalizes the library

Structures
-----------

Device (board) information

    struct usb_io_device_info
    {
	   unsigned char *serial_number;
       char *device_path;
       usb_io_device_info* next;
    };

Function `usb_io_get_device_list` returns a list of these structures (one element or more). Pointer to this struct is input for `usb_io_open_device`.

I/O pin characteristics:

    struct pin_info 
    {
       unsigned pinIndex; /* 0-15 */
       unsigned pinMode;  /* enum pin_mode */
       unsigned pinValue; /* enum pin_level */
    };
    

Enumerations
--------------

* `pin_mode` : `INPUT_MODE`, `OUTPUT_MODE`
* `pin_level` : `LOW_LEVEL`, `HIGH_LEVEL`
* `input_pin_mode` : `NO_INNNER_PULL_UP`, `INNER_PULL_UP`
* `work_led_mode` : `CLOSE_WORK_LED`, `OPEN_WORK_LED`

## Error handling

If error occurred, the API functions that return error code return -1; functions that return handles or pointers return NULL. Applications are responsible to check the returned value.


## Notes

* The list of devices is needed to call `usb_io_open_device`. Do not free it before closing all active devices (boards).
* The host must poll to detect change of inputs. The device does not generate any events or interrupts.
* The library does not support detection of hot plug/unplug of USB devices.
*  The library is not thread-safe. Applications must ensure that only one thread calls the library at any time. Several processes can use several boards, as long as each board is opened (with `usb_io_open_device`) by a single process at any time. 

TO DO
======
* ?? How to get input pin **mode** from `usb_io_get_all_pin_info`?
  * What is returned for output pins? LED?