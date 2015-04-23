How to use `usb_io_interface` library
===================================

Version 2.0

Changes to the original DLL:
----------------------------

* The type used for handles changed from `int` to `intptr_t`, for 64-bit compatibility. 
  This should be binary compatible with 32-bit programs written with the original library.
* Enumeration names changed (to fix typos; original names are kept for compatibility).
* Added helper functions for use from managed languages and scripts; see below.


C/C++ applications
--------------------------------

Include file name: `usb_io_device.h`  
Link library file name: `usb_io_interface.lib`  
Dynamic library file name: `usb_io_interface.dll`

Put the following lines in your source file:

    #include <usb_relay_device.h>

For Windows Visual C++ applications:

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
* `usb_io_set_work_led_mode` - turns the green "work" LED on or off
* `usb_io_uninit` -  Finalizes the library


### Added functions

Following functions (except of usb_io16_lib_version) are intended for scripting languages to help (or avoid) handling of native C pointers.
Native C/C++ does not need to use these.

*  `usb_io16_lib_version` - returns the library version (useful for diagnostic)
*  `usb_io_device_open_with_serial_number` - opens a device by its unique "serial number"
*  `usb_io_device_next_dev` - retrieves pointer to the next element in list of devices, as opaque value.
  This value can be passed to other functions that receive pointer to device struct.
*  `usb_io_device_get_id_string` - retrieves pointer to the "serial number" string, as opaque value.
  The caller should retrieve the string as one-byte (ASCII, zero terminated) string.
*  .... need more ... TODO

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
* `pin_level` : `LOW_LVL`, `HIGH_LVL`
* `input_pin_mode` : `NO_INNER_PULL_UP`, `INNER_PULL_UP`
* `work_led_mode` : `WORK_LED_OFF`, `WORK_LED_BLINK`

## Error handling

If error occurred, the API functions that return error code return non-0 value; functions that return handles or pointers return NULL or zero of intptr_t type. Applications are responsible to check the returned value.


## Notes

* The list of devices is needed to call `usb_io_open_device`. Do not free it before closing all active devices (boards).
* The host must poll to detect change of inputs. The device does not generate any events or interrupts.
* The library does not support detection of hot plug/unplug of USB devices.
*  The library is not thread-safe. Applications must ensure that only one thread calls the library at any time. Several processes can use several boards, as long as each board is opened (with `usb_io_open_device`) by a single process at any time. 
*  It seems that the **pullup** mode of input pins cannot be read back from the device.

