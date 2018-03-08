#include "libledger/apdu.h"
#include "libledger/error.h"
#include "libledger/transport.h"

#include "libledger/bolos/apdu.h"
#include "libledger/bolos/reset.h"

#include <stdio.h>

bool ledger_bolos_reset(struct ledger_device *device, uint16_t comm_channel_id)
{
	struct ledger_buffer buffer;
	uint8_t apdu[LEDGER_APDU_HEADER_LENGTH];

	ledger_buffer_init(&buffer, apdu, sizeof apdu);

	apdu[LEDGER_APDU_OFFSET_CLA] = LEDGER_BOLOS_APDU_CLA;
	apdu[LEDGER_APDU_OFFSET_INS] = LEDGER_BOLOS_APDU_INS_RESET;
	apdu[LEDGER_APDU_OFFSET_P1] = 0x00;
	apdu[LEDGER_APDU_OFFSET_P2] = 0x00;
	apdu[LEDGER_APDU_OFFSET_LC] = 0x00;

	if (!ledger_transport_write_apdu(device, comm_channel_id, &buffer)) {
		return false;
	}

	// TODO: read reply and check success
	struct ledger_buffer *reply = NULL;
	if (!ledger_transport_read_apdu(device, comm_channel_id, &reply)) {
		return false;
	}

	printf("reply-len: %ld\n", reply->len);
	for (int i = 0; i < reply->len; i++) {
		printf("%x ", reply->data[i]);
	}
	printf("\n");

	// TODO: check if success
	ledger_buffer_destroy(reply);

	return true;
}
