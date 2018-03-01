#ifndef LIBLEDGER_BOLOS_TARGET_ID_H_
#define LIBLEDGER_BOLOS_TARGET_ID_H_

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDGER_BOLOS_TARGET_ID_BLUE   0x31000002
#define LEDGER_BOLOS_TARGET_ID_NANO_S 0x31100002

extern int ledger_bolos_validate_target_id(struct ledger_device *device, uint16_t comm_channel_id, uint32_t target_id);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_BOLOS_TARGET_ID_H_ */
