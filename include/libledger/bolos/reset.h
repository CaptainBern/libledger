#ifndef LIBLEDGER_BOLOS_RESET_H_
#define LIBLEDGER_BOLOS_RESET_H_

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int ledger_bolos_reset(struct ledger_device *device, uint16_t comm_channel_id);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_BOLOS_RESET_H_ */
