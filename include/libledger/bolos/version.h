#ifndef LIBLEDGER_BOLOS_VERSION_H_
#define LIBLEDGER_BOLOS_VERSION_H_

#include <stdint.h>

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ledger_bolos_version {
	uint32_t target_id;
	char *os_version;
	uint8_t *flags;
	size_t flags_len;
	char *mcu_version;
};

extern int ledger_bolos_get_version(struct ledger_device *device, uint16_t comm_channel_id, struct ledger_bolos_version **version);

extern void ledger_bolos_free_version(struct ledger_bolos_version *version);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_BOLOS_VERSION_H_ */
