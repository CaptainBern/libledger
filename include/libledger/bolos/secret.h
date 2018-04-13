#ifndef LIBLEDGER_BOLOS_SECRET_H_
#define LIBLEDGER_BOLOS_SECRET_H_

#include <stdbool.h>
#include <stdint.h>

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDGER_BOLOS_NONCE_LENGTH 0x08
#define LEDGER_BOLOS_SIGNER_SERIAL_LENGTH 0x04

extern bool ledger_bolos_exchange_nonce(struct ledger_device *device, uint16_t channel_id, uint8_t nonce[LEDGER_BOLOS_NONCE_LENGTH],
		uint8_t *signer_serial[LEDGER_BOLOS_SIGNER_SERIAL_LENGTH], uint8_t *device_nonce[LEDGER_BOLOS_NONCE_LENGTH]);

extern bool ledger_bolos_get_secret(struct ledger_device *device, uint16_t channel_id);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_BOLOS_SECRET_H_ */
