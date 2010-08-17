#!/bin/sh
USB_LIBS=$(pkg-config --libs libusb-1.0)
USB_LIBS_FLAG=$(pkg-config --cflags libusb-1.0)
g++ -g -c lcontrol.cpp $USB_LIBS_FLAG $USB_LIBS
g++ -g -o lc main.cpp lcontrol.o $USB_LIBS_FLAG $USB_LIBS
