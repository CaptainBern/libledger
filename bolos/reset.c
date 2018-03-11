#include "internal/macros.h"
#include "libledger/apdu.h"
#include "libledger/error.h"
#include "libledger/transport.h"

#include "libledger/bolos/apdu.h"
#include "libledger/bolos/reset.h"

#include <stdio.h>

bool ledger_bolos_reset(struct ledger_device *device, uint16_t comm_channel_id)
{
	if (!ledger_apdu_send(device,
			LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID, LEDGER_BOLOS_APDU_CLA, LEDGER_BOLOS_APDU_INS_RESET, 0x00, 0x00, NULL)) {
		return false;
	}

	struct ledger_buffer *reply;
	uint16_t sw = 0;
	if (!ledger_apdu_read(device, LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID, &reply, &sw)) {
		return false;
	}

	printf("sw: %x\n", sw);

	return true;
}
