#include "internal/macros.h"

#include "libledger/error.h"
#include "libledger/apdu.h"
#include "libledger/bolos/constants.h"

#include "libledger/bolos/reset.h"

bool ledger_bolos_reset(struct ledger_device *device, uint16_t channel_id)
{
	struct ledger_apdu_command command = LEDGER_APDU_COMMAND_INITIALIZER(
			LEDGER_BOLOS_APDU_CLA, LEDGER_BOLOS_APDU_INS_RESET, 0x00, 0x00, NULL, 0);

	size_t data_len = 0;
	uint16_t status = 0;
	if (!ledger_apdu_exchange(device, channel_id, &command, &data_len, NULL, 0, &status))
		return false;

	if (status != 0x9000) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_OTHER);
		return false;
	}

	return true;
}
