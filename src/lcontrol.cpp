#include "lcontrol.h"
#include "fw.h"
using namespace std;

lcontrol::lcontrol() {
    ctx = NULL;
    handle = NULL;
    device = NULL;
    errorflag = 0;
    revolution[control_if] = false;
    revolution[kb_if] = false;
    int ret;
    if ((ret = libusb_init(&ctx)) != 0) {
        cerr << "Init Error " << ret << endl; //there was an error
        errorflag |= 1;
        return;
    }
	libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
    if ((handle = libusb_open_device_with_vid_pid(ctx, vendorid, productid)) == NULL) {
        cerr << "Error " << handle << " while opening device." << endl;
        errorflag |= 2;
        return;
    }
    device = libusb_get_device(handle);
    printdev(device);
    
    if (claim_if(control_if) != 0) return;
    if (claim_if(kb_if) != 0) return;

    cout << "Init done."<<endl;
}

lcontrol::~lcontrol() {
    
    if (errorflag == 0) {
        libusb_release_interface(handle, kb_if);
    }
    if (errorflag == 0) {
        libusb_release_interface(handle, control_if);
    }
    if (revolution[kb_if]) {
        libusb_attach_kernel_driver(handle, kb_if);
        cerr << "The king is back in the building." << endl;
    }
    if (revolution[control_if]) {
        libusb_attach_kernel_driver(handle, control_if);
        cerr << "The king is back in the building." << endl;
    }
    if (handle != NULL) {
        libusb_close(handle);
    }
    if (ctx != NULL) {
        libusb_exit(ctx);
    }
    cout << "Exit done." << endl;
}

void lcontrol::devlist() {
    libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
    ssize_t cnt; //holding number of devices in list
    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if(cnt < 0) {
        cout<<"Get Device Error"<<endl; //there was an error
    }
    cout<<cnt<<" Devices in list."<<endl; //print total number of usb devices
    ssize_t i; //for iterating through the list
    for(i = 0; i < cnt; i++) {
        printdev(devs[i]); //print specs of this device
    }
    libusb_free_device_list(devs, 1); //free the list, unref the devices in it
}


void lcontrol::printdev(libusb_device *dev) {
	libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) {
		cout<<"failed to get device descriptor"<<endl;
		return;
	}
	cout << "Number of possible configurations: " << (int)desc.bNumConfigurations << "  ";
	cout << "Device Class: " << (int)desc.bDeviceClass << "  ";
	cout << "VendorID: " << hex << desc.idVendor << "  ";
	cout << "ProductID: " << hex << desc.idProduct << endl;
	libusb_config_descriptor *config;
	libusb_get_config_descriptor(dev, 0, &config);
	cout << "Interfaces: " << (int)config->bNumInterfaces << endl;
	const libusb_interface *inter;
	const libusb_interface_descriptor *interdesc;
	const libusb_endpoint_descriptor *epdesc;
	for(int i=0; i<(int)config->bNumInterfaces; i++) {
		inter = &config->interface[i];
		cout << " Number of alternate settings: " << inter->num_altsetting << endl;
		for(int j=0; j<inter->num_altsetting; j++) {
			interdesc = &inter->altsetting[j];
			cout << "\t Interface Number: " << (int)interdesc->bInterfaceNumber << " | ";
			cout << "Number of endpoints: " << (int)interdesc->bNumEndpoints << endl;
			for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
				epdesc = &interdesc->endpoint[k];
				cout << "\t\t Descriptor Type: " << (int)epdesc->bDescriptorType << " | ";
				cout << "EP Address: " << (int)epdesc->bEndpointAddress << endl;
			}
		}
	}
	cout<<endl;
	libusb_free_config_descriptor(config);
}

int lcontrol::claim_if(int ifnr) {
   int ret;
   ret = libusb_kernel_driver_active(handle, ifnr);
    switch (ret) {
        case 0:
            break;

        case 1:
            cerr << "Usurping kernel driver." << endl;
            if ((ret = libusb_detach_kernel_driver(handle, ifnr)) != 0) {
                cerr << "The revolution on interface " << ifnr << " failed. Kernel driver still in place." << endl;
                errorflag |= 4;
                return errorflag;
            }
            revolution[ifnr] = true;
            break;

        default:
            cerr << "Something bad happened: " << ret << endl;
            errorflag |= 8;
            return errorflag;
            break;
    }
    if ((ret = libusb_claim_interface(handle, ifnr)) < 0) {
        cerr << "Error " << ret << " claiming interface " << ifnr << "." << endl;
        errorflag |= 16;
        return errorflag;
    }
    return 0;
}

void lcontrol::change_dpi(int dpi) {
    int ret[4];
    switch (dpi) {
        case 1:
            ret[0] = libusb_control_transfer(
                    handle,
                    0x21,
                    0x9,
                    0x210,
                    0x2,
                    fw::switch_dpi_1,
                    0x7,
                    1000);
            ret[1] = libusb_control_transfer(
                    handle,
                    0x21,
                    0x9,
                    0x210,
                    0x2,
                    fw::switch_dpi_blink,
                    0x7,
                    1000);
            ret[2] = libusb_control_transfer(
                    handle,
                    0x21,
                    0x9,
                    0x210,
                    0x2,
                    fw::switch_dpi_b1,
                    0x7,
                    1000);
            ret[3] = libusb_control_transfer(
                    handle,
                    0x21,
                    0x9,
                    0x210,
                    0x2,
                    fw::switch_dpi_blink,
                    0x7,
                    1000);
            cout << "Status: " << ret[0] << " " << ret[1] << " " << ret[2] << " " << ret[3] << endl;
            break;
        case 2:
            ret[0] = libusb_control_transfer(
                    handle,
                    0x21,
                    0x9,
                    0x210,
                    0x2,
                    fw::switch_dpi_2,
                    0x7,
                    1000);
            ret[1] = libusb_control_transfer(
                    handle,
                    0x21,
                    0x9,
                    0x210,
                    0x2,
                    fw::switch_dpi_blink,
                    0x7,
                    1000);
            ret[2] = libusb_control_transfer(
                    handle,
                    0x21,
                    0x9,
                    0x210,
                    0x2,
                    fw::switch_dpi_b2,
                    0x7,
                    1000);
            ret[3] = libusb_control_transfer(
                    handle,
                    0x21,
                    0x9,
                    0x210,
                    0x2,
                    fw::switch_dpi_blink,
                    0x7,
                    1000);
            cout << "Status: " << ret[0] << " " << ret[1] << " " << ret[2] << " " << ret[3] << endl;
            break;
        default:
            cerr << "Not implemented." << endl;
            break;
    }
}

