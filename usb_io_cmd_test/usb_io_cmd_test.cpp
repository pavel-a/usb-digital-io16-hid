// usb_io_cmd_test.cpp
//

#include <cstdio>
#include <tchar.h>
#include <Windows.h>

#include "usb_io_device.h"

#pragma comment(lib, "../Debug/usb_io_interface.lib")

int _tmain(int argc, _TCHAR* argv[])
{
    usb_io_init();

    while(1)
    {
        //get all device list
        struct usb_io_device_info *usb_io_list = NULL;
        usb_io_list = usb_io_get_device_list();

        //print all device infomation
        printf("****************************\n");
        struct usb_io_device_info *tmp = usb_io_list;
        while(tmp)
        {
            printf("serial number:%s\n", tmp->serial_number);
            tmp = tmp->next;
        }

        if (usb_io_list)
        {
            struct pin_info inof[16] = {0};
            int hand = usb_io_open_device(usb_io_list);
            if (hand)
            {
                //1. open work led
                if(usb_io_set_work_led_mode(hand, OPEN_WORK_LED) != 0)
                {
                    printf("open work led failure\n");
                    goto exit;
                }

                printf("all pin output high level\n");
                for (int i = 0; i < 16; i++)
                {
                    //set work mode
                    usb_io_set_pin_mode(hand, i, OUTPUT_MODE, NO_INNNER_PULL_UP);
                    //set out high level
                    usb_io_write_output_pin_value(hand, i, HIGHT_LEVEL);
                }

                ::Sleep(500);

                printf("all pin output low level\n");
                for (int i = 0; i < 16; i++)
                {
                    usb_io_set_pin_mode(hand, i, OUTPUT_MODE, NO_INNNER_PULL_UP);
                    usb_io_write_output_pin_value(hand, i, LOW_LEVEL);
                }

                ::Sleep(500);

            }
            else
            {
                printf("open device error...!\n");
            }
            usb_io_close_device(hand);
        }
        else
        {
            printf("find no USB IO device\n");
        }

        usb_io_free_device_list(usb_io_list);
        usb_io_uninit();

        ::Sleep(100);
    }


exit:
    ::system("PAUSE");
	return 0;
}

