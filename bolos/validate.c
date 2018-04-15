#include "internal/cursor.h"
#include "internal/macros.h"

#include "libledger/error.h"
#include "libledger/apdu.h"
#include "libledger/bolos/constants.h"

#include "libledger/bolos/validate.h"

bool ledger_bolos_validate_target_id(struct ledger_device *device, uint16_t channel_id, uint32_t target_id)
{
	uint8_t data[sizeof(target_id)];

	struct ledger_cursor cursor;

	ledger_cursor_init(&cursor, data, sizeof(data));
	ledger_cursor_write_u32(&cursor, target_id);

	struct ledger_apdu_command command = LEDGER_APDU_COMMAND_INITIALIZER(
			LEDGER_BOLOS_APDU_CLA, LEDGER_BOLOS_APDU_INS_VALIDATE, 0x0, 0x0, data, sizeof(data));

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
