#include <stdlib.h>
#include <string.h>

#include "internal/macros.h"
#include "internal/cursor.h"
#include "libledger/error.h"
#include "libledger/device.h"

#include "libledger/transport.h"

bool ledger_transport_write(struct ledger_device *device, struct ledger_transport_command *command)
{
	uint8_t transport_packet[LEDGER_TRANSPORT_MESSAGE_LENGTH];
	struct ledger_cursor out;

	ledger_cursor_init(&out, transport_packet, sizeof(transport_packet));
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

		if (!ledger_cursor_write_bytes(&out, command->payload.apdu.data, sizeof(command->payload.apdu.data))) {
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
	if (!ledger_write(device, &written, transport_packet, sizeof(transport_packet)))
		return false;

	if (written != out.buffer.len) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		return false;
	}

	return true;
}

bool ledger_transport_read(struct ledger_device *device, struct ledger_transport_reply *out, int timeout)
{
	uint8_t transport_packet[LEDGER_TRANSPORT_MESSAGE_LENGTH];
	struct ledger_cursor in;

	ledger_cursor_init(&in, transport_packet, sizeof(transport_packet));
	ledger_cursor_wipe(&in);

	size_t read = 0;
	if (!ledger_read(device, &read, transport_packet, sizeof(transport_packet), timeout))
		return false;

	if (read != in.buffer.len) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		return false;
	}

	struct ledger_transport_reply reply;
	if (!ledger_cursor_read_u16(&in, &reply.header.comm_channel_id)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
		return false;
	}

	if (!ledger_cursor_read_u8(&in, &reply.header.command_tag)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		return false;
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

		if (!ledger_cursor_read_bytes(&in, reply.payload.apdu.data, sizeof(reply.payload.apdu.data))) {
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

	if (out)
		memcpy(out, &reply, sizeof(reply));

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

	if (!ledger_transport_write(device, &command))
		return false;

	struct ledger_transport_reply reply;
	if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT))
		return false;

	if (reply.header.command_tag != LEDGER_TRANSPORT_CMD_GET_VERSION) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
		return false;
	}

	if (version)
		*version = reply.payload.version;

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

	if (!ledger_transport_write(device, &command))
		return false;

	struct ledger_transport_reply reply;
	if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT))
		return false;

	if (reply.header.command_tag != LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
		return false;
	}

	if (channel_id)
		*channel_id = reply.payload.channel_id;

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

	if (!ledger_transport_write(device, &command))
		return false;

	struct ledger_transport_reply reply;
	if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT))
		return false;

	if (reply.header.command_tag != LEDGER_TRANSPORT_CMD_PING) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
		return false;
	}

	return true;
}

bool ledger_transport_write_apdu(struct ledger_device *device, uint16_t channel_id, const uint8_t *buffer, size_t buffer_len)
{
	struct ledger_transport_command command = {
		.header = {
			.comm_channel_id = channel_id,
			.command_tag = LEDGER_TRANSPORT_CMD_APDU
		}
	};

	struct ledger_cursor out;

	ledger_cursor_init(&out, command.payload.apdu.data, sizeof(command.payload.apdu.data));

	uintptr_t written = 0;
	uint16_t sequence_id = 0;

	while (written < buffer_len) {
		ledger_cursor_wipe(&out);

		if (sequence_id == 0) {
			if (!ledger_cursor_write_u16(&out, buffer_len)) {
				LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
				return false;
			}
		}

		size_t block_len = ledger_cursor_remaining(&out);
		if (block_len > (buffer_len - written))
			block_len = buffer_len - written;

		if (!ledger_cursor_write_bytes(&out, buffer + written, block_len)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			return false;
		}

		written += block_len;

		if (!ledger_transport_write(device, &command))
			return false;

		sequence_id++;
	}

	return true;
}

bool ledger_transport_read_apdu(struct ledger_device *device, uint16_t channel_id, size_t *len, uint8_t *buffer, size_t buffer_len)
{
	struct ledger_cursor in;

	uintptr_t read = 0;
	uint16_t sequence_id = 0;
	uint16_t total_len = 0;

	memset(buffer, 0, buffer_len);

	do {
		struct ledger_transport_reply reply;
		if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT))
			return false;

		if (reply.header.comm_channel_id != channel_id) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
			return false;
		}

		if (reply.header.command_tag != LEDGER_TRANSPORT_CMD_APDU) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
			return false;
		}

		if (reply.payload.apdu.sequence_id != sequence_id) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
			return false;
		}

		ledger_cursor_init(&in, reply.payload.apdu.data, sizeof(reply.payload.apdu.data));

		if (reply.payload.apdu.sequence_id == 0) {
			if (!ledger_cursor_read_u16(&in, &total_len)) {
				LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
				return false;
			}

			*len = total_len;

			if (buffer_len < total_len) {
				LEDGER_SET_ERROR(device, LEDGER_ERROR_BUFFER_TOO_SMALL);
				return false;
			}
		}

		size_t remaining = total_len - read;
		if (remaining > ledger_cursor_remaining(&in))
			remaining = ledger_cursor_remaining(&in);

		if (!ledger_cursor_read_bytes(&in, buffer + read, remaining)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_INTERNAL);
			return false;
		}

		read += remaining;

		sequence_id++;
	} while (read < total_len);

	return true;
}
