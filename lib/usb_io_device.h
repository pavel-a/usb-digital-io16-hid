#ifndef USB_IO_DEVICE_H__
#define USB_IO_DEVICE_H__

#pragma comment(lib, "setupapi.lib")

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
    

	/*usb io board infomation structure*/
	struct usb_io_device_info
	{
		unsigned char *serial_number;
        char *device_path;
		usb_io_device_info* next;
	};

	/*init the USB IO Libary
	@returns: This function returns 0 on success and -1 on error.
	*/
	int EXPORT_API usb_io_init(void);
	
	/*Finalize the USB IO Libary.
	This function frees all of the static data associated with
	USB IO Libary. It should be called at the end of execution to avoid
	memory leaks.
	@returns:This function returns 0 on success and -1 on error.
	*/
	int EXPORT_API usb_io_uninit(void);


	/*Enumerate the USB IO Devices.
    @returns: This function returns a pointer to a linked list of type
    struct #usb_io_device_info, containing information about the usb io devices
    attached to the system, or NULL in the case of failure. Free
    this linked list by calling free_usb_io_device_list().
    */
	struct usb_io_device_info EXPORT_API * usb_io_get_device_list(void);


	/*   This function frees a linked list created by get_usb_io_device_list().*/
	void EXPORT_API usb_io_free_device_list(struct usb_io_device_info*);

	/*open a usb io device
	@return: This funcation returns a valid handle to the device on success or 0 on failure.
	*/
	int EXPORT_API  usb_io_open_device(struct usb_io_device_info *device_info);

	/*close a usb io device opend by open_usb_io_device()*/
    void EXPORT_API usb_io_close_device(int hHandle);

    /*
    open or close the green indicator  light
	@param: 
        hHandle:    the retrun value of open_usb_io_device();
		led_mode:    open or close
	@returns: returns 0 on success and others on error.
	@description:when opened the work led, the green led will flashing when works.
    */
    int EXPORT_API usb_io_set_work_led_mode(int hHandle, work_led_mode led_mode);

    /*
    set the work mode of io pin
    @param: 
        hHandle:    the retrun value of open_usb_io_device();
        pinIndex:   pin number of usb io. valid number is 0 to 15
        mode:       input mode or output mode
        innerPullUp: this param will be used when the pinIndex was set INPUT_MODE. When set param mode to INPUT_MODE and set innerPullUp to INNER_PULL_UP,
        the value (read out use read_usb_io_input_pin_value() ) of pinIndex pin  equal 1 if no input on pinIndex pin.
        We advice you to set the param innerPullUp to INNER_PULL_UP when param mode is INPUT_MODE.

    @returns: return 0 on success, others on failure.
    */
    //设置引脚输入输出模式
    int EXPORT_API usb_io_set_pin_mode(int hHandle, unsigned pinIndex, pin_mode mode, input_pin_mode innerPullUp);

    /*
    set the value of output pin
    @param:
        hHandle:    the retrun value of open_usb_io_device();
        pinIndex:   pin number of usb io. valid number is 0 to 15
        level:      low or high level
        Attention: ouputPinIndex must be a output pin that was set by set_usb_io_pin_mode() funcation.

    @returns: return 0 on success, other on failure
    */
    int EXPORT_API usb_io_write_output_pin_value(int hHandle, unsigned ouputPinIndex, pin_level level);

    /*get the value of input pin of pinIndex
    @params:
            hHandle:    the retrun value of open_usb_io_device();
            pinIndex:   pin number of usb io. valid number is 0 to 15
            level:      a non-null pointer. the value of input pin stored in this param. 0 -- low level, 1 -- high level
    @returns: return 0 on success, other on failure
    */
    int EXPORT_API usb_io_read_input_pin_value(int hHandle, unsigned pinIndex, unsigned *level);

    /*get the value of all pin
    @param:
          hHandle:    the retrun value of open_usb_io_device();  
    @returns: return 0 on success, other on failure
    */
    int EXPORT_API usb_io_get_all_pin_info(int hHandle, pin_info info[16]);

#ifdef __cplusplus
}
#endif 

#endif //end of ifdef USB_IO_DEVICE_H__