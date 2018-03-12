#include <stdlib.h>

#include "internal/macros.h"
#include "internal/cursor.h"
#include "libledger/error.h"
#include "libledger/transport.h"

#include "libledger/apdu.h"

void ledger_apdu_reply_destroy(struct ledger_apdu_reply *reply)
{
	if (reply) {
		ledger_buffer_destroy(reply->data);
		free(reply);
	}
}

bool ledger_apdu_compose(struct ledger_apdu_command *command, struct ledger_buffer *buffer)
{
	struct ledger_cursor out;

	ledger_cursor_init(&out, buffer->data, buffer->len);
	ledger_cursor_wipe(&out);

	if (!ledger_cursor_write_u8(&out, command->cla)) {
		return false;
	}

	if (!ledger_cursor_write_u8(&out, command->ins)) {
		return false;
	}

	if (!ledger_cursor_write_u8(&out, command->p1)) {
		return false;
	}

	if (!ledger_cursor_write_u8(&out, command->p2)) {
		return false;
	}

	if (command->data) {
		if (command->data->len > LEDGER_APDU_MAX_DATA_LENGTH) {
			return false;
		}

		if (!ledger_cursor_write_u8(&out, command->data->len)) {
			return false;
		}

		if (!ledger_cursor_write_bytes(&out, command->data->data, command->data->len)) {
			return false;
		}
	} else {
		if (!ledger_cursor_write_u8(&out, 0)) {
			return false;
		}
	}

	return true;
}

bool ledger_parse_apdu(struct ledger_buffer *buffer, struct ledger_apdu_reply **reply)
{
	struct ledger_cursor in;
	ledger_cursor_init(&in, buffer->data, buffer->len);

	struct ledger_apdu_reply *_reply = malloc(sizeof(struct ledger_apdu_reply));
	if (!_reply) {
		return false;
	}

	if (ledger_cursor_remaining(&in) < 2) {
		return false;
	}

	size_t apdu_data_len = ledger_cursor_remaining(&in) - 2;
	if (apdu_data_len > 0) {
		_reply->data = ledger_buffer_create(apdu_data_len);
		if (!_reply->data) {
			return false;
		}

		if (!ledger_cursor_read_bytes(&in, _reply->data->data, _reply->data->len)) {
			goto err_destroy_apdu_reply;
		}
	}

	if (!ledger_cursor_read_u16(&in, &_reply->status)) {
		goto err_destroy_apdu_reply;
	}

	*reply = _reply;

	return true;

err_destroy_apdu_reply:
	ledger_apdu_reply_destroy(_reply);
	return false;
}

bool ledger_apdu_exchange(struct ledger_device *device, uint16_t channel, struct ledger_apdu_command *command, struct ledger_apdu_reply **reply)
{
	uint8_t command_data[LEDGER_APDU_HEADER_LENGTH + (command->data ? command->data->len : 0)];
	struct ledger_buffer command_buffer;

	ledger_buffer_init(&command_buffer, command_data, sizeof command_data);

	if (!ledger_apdu_compose(command, &command_buffer)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
		return false;
	}

	if (!ledger_transport_write_apdu(device, channel, &command_buffer)) {
		return false;
	}

	struct ledger_buffer *reply_buffer;
	if (!ledger_transport_read_apdu(device, channel, &reply_buffer)) {
		return false;
	}

	if (!ledger_parse_apdu(reply_buffer, reply)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
		goto err_destroy_reply_buffer;
	}

	return true;

err_destroy_reply_buffer:
	ledger_buffer_destroy(reply_buffer);
	return false;
}
