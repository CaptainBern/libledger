#include <stdio.h>
#include <stdlib.h>

#include <hidapi/hidapi.h>
#include <libledger/libledger.h>

int main(int argc, char *argv[]) {
	struct hid_device_info *dev_info = hid_enumerate(LEDGER_VENDOR_ID, LEDGER_NANO_S_PRODUCT_ID);
	if (!dev_info) {
		printf("Failed to find any Ledger Nano S devices!\n");
		return EXIT_FAILURE;
	}

	printf("Found a Ledger Nano S: %s\n", dev_info->path);

	struct ledger_device *device = NULL;
	int ret = 0;

	ret = ledger_open_device(dev_info->path, &device);
	if (ret != LEDGER_SUCCESS) {
		printf("Failed to open device: %s\n", ledger_error_string(ret));
		return EXIT_FAILURE;
	}

	hid_free_enumeration(dev_info);

	printf("Pinging device...\n");

	ret = ledger_transport_ping(device);
	if (ret != LEDGER_SUCCESS) {
		printf("Ping failed: %s\n", ledger_error_string(ret));
		return EXIT_FAILURE;
	}

	printf("Ping success\n");

	ledger_close_device(device);

	return EXIT_SUCCESS;
}
