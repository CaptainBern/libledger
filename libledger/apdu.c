#include "internal/macros.h"
#include "internal/cursor.h"
#include "libledger/error.h"
#include "libledger/transport.h"

#include "libledger/apdu.h"

bool ledger_apdu_write(struct ledger_device *device, uint16_t channel_id, struct ledger_apdu_command *command)
{
	uint8_t apdu[LEDGER_APDU_HEADER_LENGTH + command->data_len];

	struct ledger_cursor out;

	ledger_cursor_init(&out, apdu, sizeof(apdu));
	ledger_cursor_wipe(&out);

	if (!ledger_cursor_write_u8(&out, command->cla)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
		return false;
	}

	if (!ledger_cursor_write_u8(&out, command->ins)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
		return false;
	}

	if (!ledger_cursor_write_u8(&out, command->p1)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
		return false;
	}

	if (!ledger_cursor_write_u8(&out, command->p2)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
		return false;
	}

	if (command->data_len > 0) {
		if (command->data_len > LEDGER_APDU_MAX_DATA_LENGTH) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INVALID_LENGTH);
			return false;
		}

		if (!ledger_cursor_write_u8(&out, command->data_len & 0xff)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			return false;
		}

		if (!ledger_cursor_write_bytes(&out, command->data, command->data_len)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			return false;
		}
	} else {
		if (!ledger_cursor_write_u8(&out, 0)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			return false;
		}
	}

	return ledger_transport_write_apdu(device, channel_id, apdu, sizeof(apdu));
}

bool ledger_apdu_read(struct ledger_device *device, uint16_t channel_id, size_t *len, uint8_t *buffer, size_t buffer_len, uint16_t *status)
{
	uint8_t apdu[buffer_len + 2];
	size_t apdu_len = 0;

	struct ledger_cursor in;

	if (!ledger_transport_read_apdu(device, channel_id, &apdu_len, apdu, sizeof(apdu)))
		return false;

	ledger_cursor_init(&in, apdu, apdu_len);

	size_t data_len = ledger_cursor_remaining(&in) - 2;
	if (data_len > 0) {
		if (!ledger_cursor_read_bytes(&in, buffer, data_len)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			return false;
		}
	}

	*len = data_len;

	if (!ledger_cursor_read_u16(&in, status)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
		return false;
	}

	return true;
}

bool ledger_apdu_exchange(struct ledger_device *device, uint16_t channel_id, struct ledger_apdu_command *command, size_t *len, uint8_t *buffer, size_t buffer_len, uint16_t *status)
{
	return ledger_apdu_write(device, channel_id, command) && ledger_apdu_read(device, channel_id, len, buffer, buffer_len, status);
}
