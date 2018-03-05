#ifndef LIBLEDGER_BOLOS_GET_DEPLOYED_SECRET_V2_H_
#define LIBLEDGER_BOLOS_GET_DEPLOYED_SECRET_V2_H_

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int ledger_bolos_get_deployed_secret(struct ledger_device *device,
		uint16_t comm_channel_id, uint32_t target_id);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_BOLOS_GET_DEPLOYED_SECRET_V2_H_ */
