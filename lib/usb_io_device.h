// usb_io_device.h
// *** WIP

#ifndef USB_IO_DEVICE_H__
#define USB_IO_DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API
#endif

enum pin_mode
{
    INPUT_MODE  = 0,
    OUTPUT_MODE = 1
};

enum pin_level
{
    LOW_LEVEL = 0,
    HIGHT_LEVEL = 1
};

enum input_pin_mode
{
    NO_INNNER_PULL_UP = 0,
    INNER_PULL_UP = 1
};

enum work_led_mode
{
    CLOSE_WORK_LED = 0,
    OPEN_WORK_LED = 1
};

struct pin_info
{
    unsigned pinIndex;
    unsigned pinMode;
    unsigned pinValue;
};


/* USB I/O  board information structure */
struct usb_io_device_info
{
    unsigned char *serial_number;
    char *device_path;
    usb_io_device_info* next;
};

/* Init the USB IO Library
    @return:  0 on success and -1 on error.
 */
int EXPORT_API usb_io_init(void);

/* Finalize the USB IO Library.
    This function frees all of the static data associated with
    USB IO Library. It should be called at the end of execution to avoid memory leaks.
    @return:This function returns 0 on success and -1 on error.
 */
int EXPORT_API usb_io_uninit(void);


/* Enumerate the USB IO Devices.
    @return:  pointer to a linked list of struct usb_io_device_info for all usb io devices
    attached to the system, or NULL in the case of failure.
    Free this linked list by calling free_usb_io_device_list().
    */
struct usb_io_device_info EXPORT_API * usb_io_get_device_list(void);

/*  This function frees a linked list created by get_usb_io_device_list() */
void EXPORT_API usb_io_free_device_list(struct usb_io_device_info*);

/* Open a usb I/O device
    @return: handle to the device on success or 0 on failure.
 */
intptr_t EXPORT_API usb_io_open_device(struct usb_io_device_info *device_info);

/* Close the USB device opened by open_usb_io_device() */
void EXPORT_API usb_io_close_device(intptr_t hHandle);

/* Set the green indicator  LED on or off
    @param  hHandle:  the device handle returned by usb_io_open_device
    @param  led_mode:    open or close
    @return:  0 on success and others on error.
    @note: when ON, the green LED will flash
    */
int EXPORT_API usb_io_set_work_led_mode(intptr_t hHandle, work_led_mode led_mode);

/*   set the mode of I/O pin
    @param  hHandle:  the device handle returned by usb_io_open_device
        pinIndex:   pin number (0 to 15)
        mode:       input mode or output mode
        innerPullUp: this param will be used when the pinIndex was set INPUT_MODE. 
        
    @note When set param mode to INPUT_MODE and set innerPullUp to INNER_PULL_UP,
        the value (read out use read_usb_io_input_pin_value() ) of pinIndex pin  equal 1 if no input on pinIndex pin.
        We advice you to set the param innerPullUp to INNER_PULL_UP when param mode is INPUT_MODE.

    @returns: 0 on success, others on failure.
    */
int EXPORT_API usb_io_set_pin_mode(intptr_t hHandle, unsigned pinIndex, pin_mode mode, input_pin_mode innerPullUp);

/*
   Set the value of output pin
    @param  hHandle:  the device handle returned by usb_io_open_device
        pinIndex:   pin number  (0 to 15)
        level:      low or high level
    @note  ouputPinIndex must be a output pin that was set by set_usb_io_pin_mode() function.
    @returns: 0 on success, other on failure
    */
int EXPORT_API usb_io_write_output_pin_value(intptr_t hHandle, unsigned ouputPinIndex, pin_level level);

/* Get the value of input pin of pinIndex
    @param  hHandle:  the device handle returned by usb_io_open_device
            pinIndex:   pin number (0 to 15)
            level:      a non-null pointer. the value of input pin stored in this param. 0 -- low level, 1 -- high level
    @returns: 0 on success, other on failure
    */
int EXPORT_API usb_io_read_input_pin_value(intptr_t hHandle, unsigned pinIndex, unsigned *level);

/* Get the value of all pins
    @param  hHandle:  the device handle returned by usb_io_open_device
	@param info [out]  array of pin_info
    @returns:  0 on success, other on failure
    */
int EXPORT_API usb_io_get_all_pin_info(intptr_t hHandle, pin_info info[16]);

#ifdef __cplusplus
}
#endif

#endif // USB_IO_DEVICE_H__
