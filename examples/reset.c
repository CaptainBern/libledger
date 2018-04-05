#include <stdio.h>
#include <stdlib.h>

#include "common/common.h"

int main(int argc, char *argv[]) {
	struct ledger_device *device = get_ledger_nano_s();
	if (!device) {
		printf("Ledger Nano S not found!\n");
		return EXIT_FAILURE;
	}

	if (!ledger_bolos_reset(device, LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID)) {
		printf("Reset failed: %s\n", ledger_error_str(ledger_get_error(device)));
		return EXIT_FAILURE;
	}

	ledger_close(device);

	return EXIT_SUCCESS;
}
