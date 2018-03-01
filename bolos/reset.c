#include "libledger/apdu.h"
#include "libledger/error.h"
#include "libledger/transport.h"
#include "libledger/internal/binary.h"

#include "libledger/bolos/apdu.h"
#include "libledger/bolos/reset.h"

int ledger_bolos_reset(struct ledger_device *device, uint16_t comm_channel_id) {
	int ret = 0;

	uint8_t apdu[LEDGER_APDU_HEADER_SIZE];
	apdu[LEDGER_APDU_OFFSET_CLA] = LEDGER_BOLOS_APDU_CLA;
	apdu[LEDGER_APDU_OFFSET_INS] = LEDGER_BOLOS_APDU_INS_RESET;
	apdu[LEDGER_APDU_OFFSET_P1] = 0x00;
	apdu[LEDGER_APDU_OFFSET_P2] = 0x00;
	apdu[LEDGER_APDU_OFFSET_LC] = 0x00;

	ret = ledger_transport_write_apdu(device, comm_channel_id, &apdu[0], sizeof apdu);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	struct ledger_transport_apdu_reply *reply;
	ret = ledger_transport_read_apdu(device, comm_channel_id, &reply);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	if (reply->data_len == 2) {
		if (ledger_apdu_sw_ok(reply->data[0], reply->data[1])) {
			ret = LEDGER_SUCCESS;
		} else {
			ret = LEDGER_ERROR_UNKNOWN; // TODO: better error?
		}
	} else {
		ret = LEDGER_ERROR_INCORRECT_LENGTH;
	}

	ledger_transport_free_apdu_reply(reply);
	return ret;
}
