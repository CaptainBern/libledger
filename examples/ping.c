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

	if (!ledger_transport_ping(device)) {
		printf("Ping failed: %s\n", ledger_error_str(ledger_get_error(device)));
		return EXIT_FAILURE;
	}

	printf("Ping!\n");

	ledger_close(device);

	return EXIT_SUCCESS;
}
