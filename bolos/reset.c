#include "libledger/apdu.h"
#include "libledger/error.h"
#include "libledger/transport.h"

#include "libledger/bolos/apdu.h"
#include "libledger/bolos/reset.h"

bool ledger_bolos_reset(struct ledger_device *device, uint16_t comm_channel_id)
{
	uint8_t apdu[LEDGER_APDU_HEADER_LENGTH];
	apdu[LEDGER_APDU_OFFSET_CLA] = LEDGER_BOLOS_APDU_CLA;
	apdu[LEDGER_APDU_OFFSET_INS] = LEDGER_BOLOS_APDU_INS_GET_VERSION;
	apdu[LEDGER_APDU_OFFSET_P1] = 0x00;
	apdu[LEDGER_APDU_OFFSET_P2] = 0x00;
	apdu[LEDGER_APDU_OFFSET_LC] = 0x00;

	if (ledger_transport_write_apdu(device,
			comm_channel_id, &apdu[0], sizeof apdu) != LEDGER_SUCCESS) {
		return false;
	}

	struct ledger_transport_apdu *reply = NULL;
	if (ledger_transport_read_apdu(device, comm_channel_id, &reply) != LEDGER_SUCCESS) {
		return false;
	}

	bool is_success = false;
	if (ledger_apdu_ok(reply->data, reply->len)) {
		is_success =  true;
	}

	ledger_transport_apdu_free(reply);
	return is_success;
}
