#include "libledger/apdu.h"
#include "libledger/error.h"
#include "libledger/transport.h"
#include "libledger/internal/binary.h"

#include "libledger/bolos/apdu.h"
#include "libledger/bolos/validate_target_id.h"
#include "libledger/bolos/exchange_nonce.h"
#include "libledger/bolos/get_deployed_secret.h"

int ledger_bolos_get_deployed_secret(struct ledger_device *device,
		uint16_t comm_channel_id, uint32_t target_id) {
	int ret = 0;

	ret = ledger_bolos_validate_target_id(device, comm_channel_id, target_id);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	struct ledger_bolos_nonce_exchange nonce_exchange;
	ret = ledger_bolos_exchange_nonce(device, comm_channel_id, NULL, 8, &nonce_exchange);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	return 0;
}
