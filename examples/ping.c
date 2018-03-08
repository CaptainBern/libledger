#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hidapi/hidapi.h>
#include <libledger/libledger.h>

int main(int argc, char *argv[]) {
	struct hid_device_info *dev_info = hid_enumerate(LEDGER_VENDOR_ID, LEDGER_NANO_S_PRODUCT_ID);
	if (!dev_info) {
		printf("Failed to find any Ledger Nano S devices!\n");
		return EXIT_FAILURE;
	}

	printf("Found a Ledger Nano S: %s\n", dev_info->path);

	struct ledger_device *device = ledger_open(dev_info->path);
	if (!device) {
		printf("Failed to open device!\n");
		return EXIT_FAILURE;
	}

	hid_free_enumeration(dev_info);

	// Ping the device
	printf("Pinging device...\n");
	if (!ledger_transport_ping(device)) {
		printf("Failed to ping: %s -> %s\n", ledger_error_str(ledger_get_error(device)), ledger_get_error_debug_str(device));
		return EXIT_FAILURE;
	}

	printf("\tPing success\n");

	// Allocate a communication channel
	printf("Allocating Channel:\n");

	uint16_t channel_id = 0;
	if (!ledger_transport_allocate_channel(device, &channel_id)) {
		printf("Failed to allocate channel: %s -> %s\n", ledger_error_str(ledger_get_error(device)), ledger_get_error_debug_str(device));
		return EXIT_FAILURE;
	}

	printf("\tChannel ID: 0x%x\n", channel_id);

	// Request the BOLOS version
	/*struct ledger_bolos_version *version = ledger_bolos_get_version(device, channel_id);
	if (!version) {
		printf("Failed to retrieve BOLOS version\n");
		return EXIT_FAILURE;
	}

	printf("BOLOS version info:\n");
	printf("\tTarget ID: 0x%x\n", version->target_id);
	printf("\tOS version: %s\n", version->os_version);
	printf("\tMCU version: %s\n", version->mcu_version);

	// Validate target ID
	printf("Validating target ID:\n");
	if (!ledger_bolos_validate_target_id(device, channel_id, version->target_id)) {
		printf("Failed to validate target ID\n");
		return EXIT_FAILURE;
	}

	printf("\tTarget ID validated\n");

	ledger_bolos_free_version(version);

	*/

	printf("Resetting device:\n");

	if (!ledger_bolos_reset(device, channel_id)) {
		printf("Failed to reset device: %s -> %s\n", ledger_error_str(ledger_get_error(device)), ledger_get_error_debug_str(device));
		return EXIT_FAILURE;
	}

	printf("\tReset success\n");

	ledger_close(device);

	hid_exit();

	return EXIT_SUCCESS;
}
