#ifndef LIBLEDGER_DEVICE_H_
#define LIBLEDGER_DEVICE_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEDGER_VENDOR_ID         0x2c97
#define LEDGER_BLUE_PRODUCT_ID   0x0000
#define LEDGER_NANO_S_PRODUCT_ID 0x0001

struct ledger_device;

extern int ledger_open_device(char *path, struct ledger_device **device);

extern void ledger_close_device(struct ledger_device *device);

extern int ledger_write(struct ledger_device *device, const uint8_t *data, size_t data_len, size_t *written);

extern int ledger_read(struct ledger_device *device, uint8_t *data, size_t data_len, size_t *read, int timeout);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_DEVICE_H_ */