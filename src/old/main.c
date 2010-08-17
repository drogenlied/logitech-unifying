/*
 * Set LED - program to control a USB LED device
 * from user space using libusb
 *
 * Copyright (C) 2004
 *     Greg Kroah-Hartman (greg@kroah.com)
 *
 * This program is free software; you can
 * redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the
 * License.
 *
 */
#include <stdio.h>
#include <string.h>
#include <usb.h>
#include <unistd.h>

#define NONE    0x00
#define BLUE    0x04
#define RED     0x02
#define GREEN   0x01


#define LOGITECH_VENDOR_ID   0x046d
#define UNIFYING_PRODUCT_ID  0xc52b
#define USBSTRLEN 64

const int endpoint_addr0 = 0x81;
const int endpoint_addr1 = 0x82;
const int endpoint_addr2 = 0x83;

static void change_color
        (struct usb_dev_handle *handle,
         unsigned char color)
{
    char *dummy;

    usb_control_msg(handle,
                    0x000000c8,
                    0x00000012,
                    (0x02 * 0x100) + 0x0a,
                    0xff & (~color),
                    dummy,
                    0x00000008,
                    5000);

}

static void change_resolution
        (struct usb_dev_handle *handle,
         unsigned char res)
{
    char data[4][7]={{0x10, 0x01, 0x80, 0x63, 0x8e, 0x00, 0x00},
                     {0x10, 0x01, 0x80, 0x51, 0x10, 0x11, 0x00},
                     {0x10, 0x01, 0x80, 0x51, 0x30, 0x03, 0x00},
                     {0x10, 0x01, 0x80, 0x51, 0x10, 0x11, 0x00}
                    };

    char data2[4][7]={{0x10, 0x01, 0x80, 0x63, 0x87, 0x00, 0x00},
                     {0x10, 0x01, 0x80, 0x51, 0x10, 0x11, 0x00},
                     {0x10, 0x01, 0x80, 0x51, 0x00, 0x33, 0x00},
                     {0x10, 0x01, 0x80, 0x51, 0x10, 0x11, 0x00}
                    };
    int i;
    if (res == 1) {
    for (i=0; i<4; i++) {
        usb_control_msg(handle,
                    0x00000021, //request type
                    0x00000009, //request
                    0x0210,     //value
                    0x0002,     //index
                    data[i],    //data
                    0x00000007, //size
                    5000);      //timeout
        sleep(1);
    }
    } else if (res == 2) {
      for (i=0; i<4; i++) {
        usb_control_msg(handle,
                    0x00000021, //request type
                    0x00000009, //request
                    0x0210,     //value
                    0x0002,     //index
                    data2[i],   //data
                    0x00000007, //size
                    5000);      //timeout
        sleep(1);
    }
    } else {
        printf("%s\n", "Fail.");
    }
}


static struct usb_device *device_init(void)
{
    struct usb_bus *usb_bus;
    struct usb_device *dev;

    usb_init();
    usb_find_busses();
    usb_find_devices();

    for (usb_bus = usb_busses;
         usb_bus;
         usb_bus = usb_bus->next) {
        for (dev = usb_bus->devices;
             dev;
             dev = dev->next) {
            if ((dev->descriptor.idVendor
                  == LOGITECH_VENDOR_ID) &&
                (dev->descriptor.idProduct
                  == UNIFYING_PRODUCT_ID))
                return dev;
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    struct usb_device *usb_dev;
    struct usb_dev_handle *usb_handle;
    int retval = 1;
    int i;
    int usb_strlen[2];
    unsigned char res = 0;
    char usb_str[2][USBSTRLEN];

    usb_dev = device_init();
    if (usb_dev == NULL) {
        fprintf(stderr, "Device not found\n");
        exit(1);
    }

    usb_handle = usb_open(usb_dev);
    if (usb_handle == NULL) {
        fprintf(stderr,
             "Not able to claim the USB device\n");
        goto exit;
    }
    
   /* usb_strlen[0] = usb_get_string_simple(usb_handle, 1, usb_str[0], USBSTRLEN);
    usb_strlen[1] = usb_get_string_simple(usb_handle, 2, usb_str[1], USBSTRLEN);
    if (usb_strlen[0] < 0 ||
        usb_strlen[0] < 0)
        {
        fprintf(stderr,
             "Not able to get dev string\n");
        goto exit;
    }
    printf("Found: %s %s\n", usb_str[0], usb_str[1]);
*/
    if (argc == 1) {
        fprintf(stderr,
                "specify at least 1 arg\n");
        goto exit;
    }

    for (i = 1; i < argc; ++i) {
        if (strcasecmp(argv[i], "a") == 0)
            res = 1;
        if (strcasecmp(argv[i], "b") == 0)
            res = 2;
        if (strcasecmp(argv[i], "c") == 0)
            res = 3;
        if (strcasecmp(argv[i], "d") == 0)
            res = 4;
    }
    change_resolution(usb_handle, res);
    retval = 0;

exit:
    usb_close(usb_handle);
    return retval;
}

