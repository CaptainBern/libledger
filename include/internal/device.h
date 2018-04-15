#ifndef LIBLEDGER_INTERNAL_DEVICE_H_
#define LIBLEDGER_INTERNAL_DEVICE_H_

#include <stddef.h>
#include <stdint.h>

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void ledger_set_error(struct ledger_device *device, int error, char *error_debug_str);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_INTERNAL_DEVICE_H_ */
