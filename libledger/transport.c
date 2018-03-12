#include <stdlib.h>
#include <string.h>

#include "internal/macros.h"
#include "internal/cursor.h"
#include "libledger/error.h"
#include "libledger/device.h"

#include "libledger/transport.h"

bool ledger_transport_write(struct ledger_device *device, struct ledger_transport_command *command)
{
	uint8_t transport_packet[LEDGER_TRANSPORT_PACKET_LENGTH];
	struct ledger_cursor out;

	ledger_cursor_init(&out, transport_packet, sizeof transport_packet);
	ledger_cursor_wipe(&out);

	if (!ledger_cursor_write_u16(&out, command->header.comm_channel_id)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
		return false;
	}

	if (!ledger_cursor_write_u8(&out, command->header.command_tag)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
		return false;
	}

	switch (command->header.command_tag) {
	case LEDGER_TRANSPORT_CMD_GET_VERSION:
	case LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL:
	case LEDGER_TRANSPORT_CMD_PING: {
		break;
	}
	case LEDGER_TRANSPORT_CMD_APDU: {
		if (!ledger_cursor_write_u16(&out, command->payload.apdu.sequence_id)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			return false;
		}

		if (!ledger_cursor_write_bytes(&out, command->payload.apdu.data, sizeof command->payload.apdu.data)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			return false;
		}
		break;
	}
	default:
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INVALID_COMMAND);
		return false;
	}

	size_t written = 0;
	if (!ledger_write(device, &out.buffer, &written)) {
		return false;
	}

	if (written != out.buffer.len) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		return false;
	}

	return true;
}

bool ledger_transport_read(struct ledger_device *device, struct ledger_transport_reply *out, int timeout)
{
	uint8_t transport_packet[LEDGER_TRANSPORT_PACKET_LENGTH];
	struct ledger_cursor in;

	ledger_cursor_init(&in, transport_packet, sizeof transport_packet);
	ledger_cursor_wipe(&in);

	size_t read = 0;
	if (!ledger_read(device, &in.buffer, &read, timeout)) {
		return false;
	}

	if (read != in.buffer.len) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		return false;
	}

	struct ledger_transport_reply reply;
	if (!ledger_cursor_read_u16(&in, &reply.header.comm_channel_id)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
	}

	if (!ledger_cursor_read_u8(&in, &reply.header.command_tag)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
	}

	switch (reply.header.command_tag) {
	case LEDGER_TRANSPORT_CMD_GET_VERSION: {
		if (!ledger_cursor_read_u32(&in, &reply.payload.version)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			return false;
		}
		break;
	}
	case LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL: {
		if (!ledger_cursor_read_u16(&in, &reply.payload.channel_id)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			return false;
		}
		break;
	}
	case LEDGER_TRANSPORT_CMD_PING: {
		// ping does not have any data
		break;
	}
	case LEDGER_TRANSPORT_CMD_APDU: {
		if (!ledger_cursor_read_u16(&in, &reply.payload.apdu.sequence_id)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
			return false;
		}

		if (!ledger_cursor_read_bytes(&in, reply.payload.apdu.data, sizeof reply.payload.apdu.data)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
			return false;
		}
		break;
	}
	default: {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INVALID_COMMAND);
		return false;
	}
	}

	if (out) {
		memcpy(out, &reply, sizeof reply);
	}

	return true;
}

bool ledger_transport_get_version(struct ledger_device *device, uint32_t *version)
{
	struct ledger_transport_command command = {
		.header = {
			.comm_channel_id = LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID,
			.command_tag = LEDGER_TRANSPORT_CMD_GET_VERSION
		}
	};

	if (!ledger_transport_write(device, &command)) {
		return false;
	}

	struct ledger_transport_reply reply;
	if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT)) {
		return false;
	}

	if (reply.header.command_tag != LEDGER_TRANSPORT_CMD_GET_VERSION) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
		return false;
	}

	if (version) {
		*version = reply.payload.version;
	}

	return true;
}

bool ledger_transport_allocate_channel(struct ledger_device *device, uint16_t *channel_id)
{
	struct ledger_transport_command command = {
		.header = {
			.comm_channel_id = LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID,
			.command_tag = LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL
		}
	};

	if (!ledger_transport_write(device, &command)) {
		return false;
	}

	struct ledger_transport_reply reply;
	if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT)) {
		return false;
	}

	if (reply.header.command_tag != LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
		return false;
	}

	if (channel_id) {
		*channel_id = reply.payload.channel_id;
	}

	return true;
}

bool ledger_transport_ping(struct ledger_device *device)
{
	struct ledger_transport_command command = {
		.header = {
			.comm_channel_id = LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID,
			.command_tag = LEDGER_TRANSPORT_CMD_PING
		}
	};

	if (!ledger_transport_write(device, &command)) {
		return false;
	}

	struct ledger_transport_reply reply;
	if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT)) {
		return false;
	}

	if (reply.header.command_tag != LEDGER_TRANSPORT_CMD_PING) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
		return false;
	}

	return true;
}

bool ledger_transport_write_apdu(struct ledger_device *device, uint16_t channel_id, const struct ledger_buffer *apdu)
{
	struct ledger_transport_command command = {
		.header = {
			.comm_channel_id = channel_id,
			.command_tag = LEDGER_TRANSPORT_CMD_APDU
		}
	};

	struct ledger_cursor out;
	struct ledger_cursor in;

	ledger_cursor_init(&out, command.payload.apdu.data, sizeof command.payload.apdu.data);
	ledger_cursor_init(&in, apdu->data, apdu->len);

	uint16_t sequence_id = 0;
	while (ledger_cursor_remaining(&in) > 0) {
		ledger_cursor_wipe(&out);

		if (sequence_id == 0) {
			if (!ledger_cursor_write_u16(&out, apdu->len)) {
				LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
				return false;
			}
		}

		size_t block_len = ledger_cursor_available(&out);
		if (block_len > ledger_cursor_remaining(&in)) {
			block_len = ledger_cursor_remaining(&in);
		}

		if (!ledger_cursor_copy(&out, &in, block_len)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			return false;
		}

		if (!ledger_transport_write(device, &command)) {
			return false;
		}

		sequence_id++;
	}

	return true;
}

bool ledger_transport_read_apdu(struct ledger_device *device, uint16_t channel_id, struct ledger_buffer **apdu)
{
	struct ledger_buffer *buffer = NULL;

	struct ledger_cursor in;
	struct ledger_cursor out;

	uint16_t sequence_id = 0;
	do {
		struct ledger_transport_reply reply;
		if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT)) {
			goto err_destroy_buffer;
		}

		if (reply.header.comm_channel_id != channel_id) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
			goto err_destroy_buffer;
		}

		if (reply.header.command_tag != LEDGER_TRANSPORT_CMD_APDU) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
			goto err_destroy_buffer;
		}

		if (reply.payload.apdu.sequence_id != sequence_id) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
			goto err_destroy_buffer;
		}

		ledger_cursor_init(&in, reply.payload.apdu.data, sizeof reply.payload.apdu.data);
		if (reply.payload.apdu.sequence_id == 0) {
			uint16_t total_len = 0;
			if (!ledger_cursor_read_u16(&in, &total_len)) {
				LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
				return false;
			}

			buffer = ledger_buffer_create(total_len);
			if (!buffer) {
				LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
				return false;
			}

			ledger_cursor_init(&out, buffer->data, buffer->len);
		}

		size_t remaining = ledger_cursor_remaining(&out);
		if (remaining > ledger_cursor_available(&in)) {
			remaining = ledger_cursor_available(&in);
		}

		if (!ledger_cursor_copy(&out, &in, remaining)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			goto err_destroy_buffer;
		}

		sequence_id++;
	} while (ledger_cursor_available(&out) > 0);

	*apdu = buffer;

	return true;

err_destroy_buffer:
	ledger_buffer_destroy(buffer);
	return false;
}
