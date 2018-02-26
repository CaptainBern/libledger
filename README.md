# Libledger

A simple library to interface with [Ledger](https://www.ledgerwallet) hardware wallets. Supporting the Ledger Nano S and Ledger Blue.

## Building

Install the dependencies:

* HidAPI (libhidapi-libusb)

Run the following commands:

    meson builddir
    ninja -C builddir

## Example

```C
#include <stdlib.h>
#include <stdio.h>

#include <hidapi/hidapi.h>
#include <libledger/libledger.h>

int main(int argc, char **argv) {
    struct ledger_device *ledger_device = NULL;
    struct hid_device_info *hid_devices;

    hid_devices = hid_enumerate(LEDGER_VENDOR_ID, LEDGER_NANO_S_PRODUCT_ID);

    /**
     * For demonstration purposes we only care about the first device.
     * If that fails to open, we don't bother checking for the others.
     */
    if (hid_devices != NULL) {
        printf("Found a Ledger Nano S device\n");
        
        int err = ledger_open_device(curr_hid_device->path, &ledger_device);
        if (err != LEDGER_SUCCESS) {
            printf("Failed to open device: %s\n", ledger_error_string(err));
            return -1;
        }

        break;
    }

    hid_free_enumeration(hid_devices);

    int ret = 0;

    // Try to ping the device
    ret = ledger_transport_ping(device);
    if (ret != LEDGER_SUCCESS) {
        printf("Failed to ping the device: %s\n", ledger_error_string(ret));
    }

    ledger_close_device(device);

    hid_exit();
}

```
