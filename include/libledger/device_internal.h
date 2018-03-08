#ifndef LIBLEDGER_DEVICE_INTERNAL_H_
#define LIBLEDGER_DEVICE_INTERNAL_H_

#include <stddef.h>
#include <stdint.h>

#include "libledger/buffer.h"

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void ledger_set_error(struct ledger_device *device, int error, char *error_debug_str);

extern bool ledger_write(struct ledger_device *device, const struct ledger_buffer *buffer, size_t *written);

extern bool ledger_read(struct ledger_device *device, struct ledger_buffer *buffer, size_t *read, int timeout);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_DEVICE_INTERNAL_H_ */
