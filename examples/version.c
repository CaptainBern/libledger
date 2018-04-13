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

	struct ledger_bolos_version *version = NULL;
	if (!ledger_bolos_get_version(device, LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID, &version)) {
		printf("Failed to retrieve version: %s\n", ledger_error_str(ledger_get_error(device)));
		return EXIT_FAILURE;
	}

	printf("BOLOS version:\n");
	printf("\ttarget ID: %x\n", version->target_id);
	printf("\tOS version: %s\n", version->os_version);
	printf("\tMCU version: %s\n", version->mcu_version);
	printf("\tMCU hash: %s\n", version->mcu_hash.len > 0 ? "yes" : "no");

	ledger_bolos_version_destroy(version);

	ledger_close(device);

	return EXIT_SUCCESS;
}
