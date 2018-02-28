#ifndef LIBLEDGER_ETHEREUM_APP_H_
#define LIBLEDGER_ETHEREUM_APP_H_

#include <stdint.h>

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ledger_ethereum_app_config {
	uint8_t flags;
	uint8_t version_major;
	uint8_t version_minor;
	uint8_t version_patch;
};

/**
 * Retrieve the Ethereum App configuration from the device.
 *
 * Note that browser support needs to be disabled in the Ethereum App for this to work.
 */
extern int ledger_ethereum_app_get_config(struct ledger_device *device, struct ledger_ethereum_app_config *config);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_ETHEREUM_APP_H_ */
