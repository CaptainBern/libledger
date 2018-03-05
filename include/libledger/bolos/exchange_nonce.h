#ifndef LIBLEDGER_BOLOS_EXCHANGE_NONCE_H_
#define LIBLEDGER_BOLOS_EXCHANGE_NONCE_H_

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDGER_BOLOS_SIGNER_SERIAL_LENGTH 4
#define LEDGER_BOLOS_NONCE_LENGTH 8

struct ledger_bolos_nonce_exchange {
	uint8_t signer_serial[LEDGER_BOLOS_SIGNER_SERIAL_LENGTH];
	uint8_t device_nonce[LEDGER_BOLOS_NONCE_LENGTH];
};

extern int ledger_bolos_exchange_nonce(struct ledger_device *device, uint16_t comm_channel_id,
		uint8_t *nonce, size_t nonce_len, struct ledger_bolos_nonce_exchange *exchange);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_BOLOS_EXCHANGE_NONCE_H_ */
