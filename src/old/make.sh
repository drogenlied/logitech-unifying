#!/bin/sh
gcc -o usbtest main.c -g -lusb
gcc -o usbinfo usbinfo.c -g -lusb
