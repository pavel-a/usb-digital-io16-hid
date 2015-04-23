// usb_io_cmd_test.cpp

#if defined(WIN32) || defined(_WIN32)

#include <targetver.h>
#include <Windows.h>

static void delayMs(int ms) { ::Sleep(ms); }

#ifdef _MSC_VER
#pragma comment(lib, "usb_io_interface.lib")
#endif //MSC_VER

#endif //WIN32

#include <cstdio>
#include <tchar.h>

#include <usb_io_device.h>

typedef intptr_t devhnd_t; // type for dev. handle

///////////////////////////////////////////////////

int enum_test()
{
    printf("Enumerating devices...\n");
    usb_io_device_info *usb_io_list = NULL;
    int n = 0;
    usb_io_list = usb_io_get_device_list();
    if (!usb_io_list) {
        printf("No devices found\n");
        return 1;
    }

    for(usb_io_device_info *tmp = usb_io_list;
        tmp;
        tmp = tmp->next ) {
        n++;
        printf("%d: serial number:%s\n", n, tmp->serial_number);
    }

    printf("Found %d devices\n", n);
    usb_io_free_device_list(usb_io_list);
    return 0;
}

///////////////////////////////////////////////////
// Wrapper for calling test on 1st found device
int one_dev_test( int (*ftest)(devhnd_t a_dev, intptr_t a_param), intptr_t param )
{
    int rc = -1;
    usb_io_device_info *usb_io_list = NULL;
    usb_io_list = usb_io_get_device_list();
    if (!usb_io_list) {
        printf("No devices found\n");
        return rc;
    }

    for(struct usb_io_device_info *tmp = usb_io_list;
        tmp;
        tmp = tmp->next ) {
        printf("Running on device with serial number:%s\n", tmp->serial_number);

        if (ftest) {
          devhnd_t hnd = usb_io_open_device(usb_io_list);
          try {
              rc = ftest( hnd, param );
          } catch(...) {
              printf("\nException!\n");
          }
          usb_io_close_device(hnd);
        }

        break; //only 1st device
    }

    usb_io_free_device_list(usb_io_list);
    return rc;
}

///////////////////////////////////////////////////

int test_LED_on_off(devhnd_t dev, intptr_t param)
{
    if ( usb_io_set_work_led_mode(dev, WORK_LED_BLINK) != 0 ) {
        printf("led on failed\n");
        return 2;
    }

    int ms = (int)param;
    if ( ms < 100 ) ms = 100;
    if ( ms > 10000 ) ms = 10000;
    delayMs(ms);

    if ( usb_io_set_work_led_mode(dev, WORK_LED_OFF) != 0 ) {
        printf("close work led off failed\n");
        return 2;
    }

    return 0;
}

// I/O pin loopback test
// Specified in and out pins should be connected.
int test_IO_loopbk(devhnd_t dev, intptr_t param)
{
    unsigned pin_IN = param & 0xF;         // ....000N
    unsigned pin_OUT = (param >> 4) & 0xF; // ....00N0
    input_pin_mode ipm = INNER_PULL_UP; // INNER_PULL_UP|NO_INNER_PULL_UP
    int rc = -1;
    unsigned inp = -1;
    int loop_cnt = 3;

    printf("Loopback test: Connect pins OUT[%u] -> IN[%u] [pullup mode=%d]\n", pin_IN, pin_OUT, (int)ipm);
    rc = usb_io_set_pin_mode(dev, pin_IN, INPUT_MODE, ipm);
    rc = usb_io_set_pin_mode(dev, pin_OUT, OUTPUT_MODE, NO_INNER_PULL_UP);

    // Read input
    rc = usb_io_read_input_pin_value(dev, pin_IN, &inp);
    printf("? IN[%d]=%X\n", pin_IN, inp);

    for (int j = loop_cnt; j > 0; --j) {
        // Set output low, read
        rc = usb_io_write_output_pin_value(dev, pin_OUT, LOW_LVL);
        rc = usb_io_read_input_pin_value(dev, pin_IN, &inp);
        printf("v IN[%d]=%X\n", pin_IN, inp);

        // Set output high, read
        delayMs(200);
        rc = usb_io_write_output_pin_value(dev, pin_OUT, HIGH_LVL);
        delayMs(200);
        rc = usb_io_read_input_pin_value(dev, pin_IN, &inp);
        printf("^ IN[%d]=%X\n", pin_IN, inp);

    } //loop

    return 0;
}

// Read status test
int test_read_status(devhnd_t dev, intptr_t param)
{
    pin_info pi[16+2];
    //1. read 1st time before any sets
    int rc = -1;
    memset(pi, 0xEE, sizeof(pi));
    printf("-------- read 1 -----------\n");
    rc = usb_io_get_all_pin_info(dev, &pi[0]);
    for (int k=0; k < 16; k++) {
        pin_info *p = &pi[k];
        printf("[%u]: mode=%u val=%u\n", p->pinIndex, p->pinMode, p->pinValue);
    }

    //2. Make some settings and read again
    rc = usb_io_set_pin_mode(dev, 0, INPUT_MODE, NO_INNER_PULL_UP);
    rc = usb_io_set_pin_mode(dev, 1, INPUT_MODE, INNER_PULL_UP);
    rc = usb_io_set_pin_mode(dev, 4, OUTPUT_MODE, NO_INNER_PULL_UP);
    rc = usb_io_set_pin_mode(dev, 5, INPUT_MODE, INNER_PULL_UP);
    rc = usb_io_set_pin_mode(dev, 8, INPUT_MODE, INNER_PULL_UP);
    rc = usb_io_set_pin_mode(dev, 9, INPUT_MODE, NO_INNER_PULL_UP);
    rc = usb_io_set_pin_mode(dev, 10, OUTPUT_MODE, NO_INNER_PULL_UP);
    rc = usb_io_set_pin_mode(dev, 12, OUTPUT_MODE, NO_INNER_PULL_UP);

    rc = usb_io_write_output_pin_value(dev, 4, HIGH_LVL);
    rc = usb_io_write_output_pin_value(dev, 12, HIGH_LVL);
    rc = usb_io_write_output_pin_value(dev, 10, LOW_LVL);

    rc = -1;
    memset(pi, 0xEE, sizeof(pi));
    delayMs(50);
    printf("-------- read 2 -----------\n");
    rc = usb_io_get_all_pin_info(dev, &pi[0]);
    for (int k=0; k < 16; k++) {
        pin_info *p = &pi[k];
        printf("[%u]: mode=%u val=%u\n", p->pinIndex, p->pinMode, p->pinValue);
    }

    return 0;
}

///////////////////////////////////////////////////

int orig_loop_test()
{
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
            devhnd_t hand = usb_io_open_device(usb_io_list);
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
                    usb_io_set_pin_mode(hand, i, OUTPUT_MODE, NO_INNER_PULL_UP);
                    //set out high level
                    usb_io_write_output_pin_value(hand, i, HIGHT_LEVEL);
                }

                delayMs(500);

                printf("all pin output low level\n");
                for (int i = 0; i < 16; i++)
                {
                    usb_io_set_pin_mode(hand, i, OUTPUT_MODE, NO_INNNER_PULL_UP);
                    usb_io_write_output_pin_value(hand, i, LOW_LEVEL);
                }

                delayMs(500);

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
    }

    exit:
    return 0;
}


int main(int argc, char* argv[])
{
    int rc = 0;
    usb_io_init();

    try {

//      rc = one_dev_test( test_read_status, -1);
//    rc = orig_loop_test();
//    rc = enum_test();
//      rc = one_dev_test( test_LED_on_off, 2500 );

      //Loopback pins ## 0 <-> 10:
      rc = one_dev_test( test_IO_loopbk, (0U) | (10U << 4));
    } catch(...) {
        printf("\nException!\n");
    }

    delayMs(100);
    usb_io_uninit();

    return rc;
}
