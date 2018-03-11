#include <string.h>

#include "internal/macros.h"
#include "internal/cursor.h"
#include "libledger/error.h"
#include "libledger/transport.h"

#include "libledger/apdu.h"

bool ledger_apdu_send(struct ledger_device *device, uint16_t comm_channel_id, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, struct ledger_buffer *data)
{
	size_t lc = data ? data->len : 0;
	if (lc > LEDGER_APDU_MAX_DATA_LENGTH) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INVALID_LENGTH);
		return false;
	}

	uint8_t apdu[LEDGER_APDU_HEADER_LENGTH + lc];
	apdu[LEDGER_APDU_OFFSET_CLA] = cla;
	apdu[LEDGER_APDU_OFFSET_INS] = ins;
	apdu[LEDGER_APDU_OFFSET_P1] = p1;
	apdu[LEDGER_APDU_OFFSET_P2] = p2;
	apdu[LEDGER_APDU_OFFSET_LC] = lc;

	if (data) {
		memcpy(&apdu[LEDGER_APDU_OFFSET_CDATA], data->data, data->len);
	}

	struct ledger_buffer buffer;
	ledger_buffer_init(&buffer, apdu, sizeof apdu);

	return ledger_transport_write_apdu(device, comm_channel_id, &buffer);
}

bool ledger_apdu_read(struct ledger_device *device, uint16_t comm_channel_id, struct ledger_buffer **data, uint16_t *sw)
{
	struct ledger_buffer *apdu = NULL;
	if (!ledger_transport_read_apdu(device, comm_channel_id, &apdu)) {
		return false;
	}

	if (apdu->len < 2) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INVALID_LENGTH);
		goto err_destroy_apdu;
	}

	struct ledger_cursor cursor;
	ledger_cursor_init(&cursor, apdu->data, apdu->len);

	if (sw) {
		ledger_cursor_skip_read(&cursor, ledger_cursor_remaining(&cursor) - 2);
		if (!ledger_cursor_read_u16(&cursor, sw)) {
			goto err_destroy_apdu;
		}
		ledger_cursor_reset_read(&cursor);
	}

	ledger_buffer_resize(apdu, ledger_cursor_available(&cursor) - 2);
	*data = apdu;
	return true;
err_destroy_apdu:
	ledger_buffer_destroy(apdu);
	return false;
}
