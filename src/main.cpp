#include <iostream>
#include <libusb.h>
#include "lcontrol.h"

using namespace std;

int main() {
    lcontrol *c;
    c = new lcontrol();
    //c->change_dpi(2);
    c->change_dpi(1);
    delete c;
	return 0;
}

