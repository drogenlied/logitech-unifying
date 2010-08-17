#include <iostream>
#include <libusb.h>

using namespace std;

class lcontrol {
    public:
        lcontrol();
        ~lcontrol();
        void devlist();
        void change_dpi(int dpi);
    private:
        void printdev(libusb_device *dev); //prototype of the function
        int claim_if(int ifnr);
        static const uint16_t vendorid  = 0x046d;
        static const uint16_t productid  = 0xc52b;
        static const int control_if  = 0;
        static const int ms_if  = 1;
        static const int kb_if  = 2;
        libusb_context *ctx; // libusb session
        libusb_device_handle *handle; // handle
        libusb_device *device; // device

        int errorflag;
        bool revolution[3];
};
