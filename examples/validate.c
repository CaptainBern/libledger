#include <stdio.h>
#include <stdlib.h>

#include "common/common.h"

int main(int argc, char *argv[])
{
	struct ledger_device *device = get_ledger_nano_s();
	if (!device) {
		printf("Ledger Nano S not found!\n");
		return EXIT_FAILURE;
	}

	uint32_t target_id = 0x31100003;
	if (!ledger_bolos_validate_target_id(device, LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID, target_id)) {
		printf("Failed to validate: %s -> %s\n", ledger_error_str(ledger_get_error(device)), ledger_get_error_debug_str(device));
		return EXIT_FAILURE;
	}

	ledger_close(device);

	return EXIT_SUCCESS;
}
