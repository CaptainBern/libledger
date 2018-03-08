#include "libledger/apdu.h"
#include "libledger/error.h"
#include "libledger/transport.h"

#include "internal/binary.h"

#include "libledger/bolos/apdu.h"
#include "libledger/bolos/validate_target_id.h"

bool ledger_bolos_validate_target_id(struct ledger_device *device,
		uint16_t comm_channel_id, uint32_t target_id)
{
	uint8_t apdu[LEDGER_APDU_HEADER_LENGTH + 4];
	apdu[LEDGER_APDU_OFFSET_CLA] = LEDGER_BOLOS_APDU_CLA;
	apdu[LEDGER_APDU_OFFSET_INS] = LEDGER_BOLOS_APDU_INS_VALIDATE_TARGET_ID;
	apdu[LEDGER_APDU_OFFSET_P1] = 0x00;
	apdu[LEDGER_APDU_OFFSET_P2] = 0x00;
	apdu[LEDGER_APDU_OFFSET_LC] = sizeof target_id;
	binary_hton_uint32(&apdu[LEDGER_APDU_OFFSET_CDATA], target_id);

	if (ledger_transport_write_apdu(device,
			comm_channel_id, &apdu[0], sizeof apdu) != LEDGER_SUCCESS) {
		return false;
	}

	struct ledger_transport_apdu *reply = NULL;
	if (ledger_transport_read_apdu(device,
			comm_channel_id, &reply) != LEDGER_SUCCESS) {
		return false;
	}

	bool is_success = false;
	if (ledger_apdu_ok(reply->data, reply->len)) {
		is_success = true;
	}
	ledger_transport_apdu_free(reply);
	return is_success;
}
