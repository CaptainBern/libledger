#include "common.h"

struct ledger_device *get_ledger_nano_s(void)
{
	struct ledger_device *ledger_device = NULL;

	struct hid_device_info *device_info = hid_enumerate(LEDGER_VENDOR_ID, LEDGER_NANO_S_PRODUCT_ID);
	if (device_info) {
		ledger_device = ledger_open(device_info->path);
	}

	hid_free_enumeration(device_info);

	return ledger_device;
}
