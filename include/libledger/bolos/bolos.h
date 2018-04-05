#ifndef LIBLEDGER_BOLOS_H_
#define LIBLEDGER_BOLOS_H_

#include <stdbool.h>

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ledger_bolos_version {
	uint32_t target_id;

	char *os_version;

	struct {
		size_t len;
		uint8_t *flags;
	} flags;

	char *mcu_version;

	struct {
		size_t len;
		uint8_t *hash;
	} mcu_hash;
};

extern void ledger_bolos_version_destroy(struct ledger_bolos_version *version);

extern bool ledger_bolos_get_version(struct ledger_device *device, uint16_t channel_id, struct ledger_bolos_version **version);

extern bool ledger_bolos_reset(struct ledger_device *device, uint16_t channel_id);

extern bool ledger_bolos_validate_target_id(struct ledger_device *device, uint16_t channel_id, uint32_t target_id);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_BOLOS_H_ */
