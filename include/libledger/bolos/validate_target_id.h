#ifndef LIBLEDGER_BOLOS_VALIDATE_TARGET_ID_H_
#define LIBLEDGER_BOLOS_VALIDATE_TARGET_ID_H_

#include <stdbool.h>

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

extern bool ledger_bolos_validate_target_id(struct ledger_device *device,
		uint16_t comm_channel_id,  uint32_t target_id);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_BOLOS_VALIDATE_TARGET_ID_H_ */
