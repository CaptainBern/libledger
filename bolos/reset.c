#include "internal/macros.h"
#include "libledger/error.h"
#include "libledger/apdu.h"
#include "libledger/bolos/apdu.h"

#include "libledger/bolos/bolos.h"

bool ledger_bolos_reset(struct ledger_device *device, uint16_t channel_id)
{
	struct ledger_apdu_command command = LEDGER_APDU_COMMAND_INITIALIZER(
			LEDGER_BOLOS_APDU_CLA, LEDGER_BOLOS_APDU_INS_RESET, 0x00, 0x00, NULL);

	struct ledger_apdu_reply *reply = NULL;
	if (!ledger_apdu_exchange(device, channel_id, &command, &reply))
		return false;

	bool success = reply->status == 0x9000;
	if (!success)
		LEDGER_SET_ERROR(device, LEDGER_ERROR_OTHER);

	ledger_apdu_reply_destroy(reply);

	return success;
}
