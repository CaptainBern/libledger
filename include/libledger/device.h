#ifndef LIBLEDGER_DEVICE_H_
#define LIBLEDGER_DEVICE_H_

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEDGER_VENDOR_ID         0x2c97
#define LEDGER_BLUE_PRODUCT_ID   0x0000
#define LEDGER_NANO_S_PRODUCT_ID 0x0001

struct ledger_device;

extern struct ledger_device *ledger_open(char *path);

extern void ledger_close(struct ledger_device *device);

extern bool ledger_write(struct ledger_device *device, const struct ledger_buffer *buffer, size_t *written);

extern bool ledger_read(struct ledger_device *device, struct ledger_buffer *buffer, size_t *read, int timeout);

extern int ledger_get_error(struct ledger_device *device);

extern char *ledger_get_error_debug_str(struct ledger_device *device);

extern void ledger_clear_error(struct ledger_device *device);


#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_DEVICE_H_ */
