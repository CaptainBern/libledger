#include <stdlib.h>
#include <string.h>

#include "internal/macros.h"
#include "internal/binary.h"
#include "libledger/error.h"
#include "libledger/device_internal.h"

#include "libledger/transport.h"

bool ledger_transport_write(struct ledger_device *device, struct ledger_transport_command *command)
{
	uint8_t buffer[LEDGER_TRANSPORT_PACKET_LENGTH];
	size_t offset = 0;

	memset(&buffer, 0, sizeof buffer);
	offset += binary_hton_uint16(&buffer[offset], command->comm_channel_id);
	offset += binary_hton_uint8(&buffer[offset], command->command_tag);

	switch (command->command_tag) {
	case LEDGER_TRANSPORT_CMD_GET_VERSION:
	case LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL:
	case LEDGER_TRANSPORT_CMD_PING: {
		break;
	}
	case LEDGER_TRANSPORT_CMD_APDU: {
		offset += binary_hton_uint16(&buffer[offset], command->apdu_part->sequence_id);
		offset += binary_hton(&buffer[offset], command->apdu_part->data, sizeof command->apdu_part->data);
		break;
	}
	default:
		LEDGER_SET_ERROR(device, LEDGER_ERROR_INVALID_COMMAND);
		return false;
	}

	size_t written = 0;
	if (!ledger_write(device, &buffer[0], sizeof buffer, &written)) {
		return false;
	}

	if (written != sizeof buffer) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		return false;
	}

	return true;
}

bool ledger_transport_read(struct ledger_device *device, struct ledger_transport_reply *out, int timeout)
{
	uint8_t buffer[LEDGER_TRANSPORT_PACKET_LENGTH];
	size_t offset = 0;

	size_t read = 0;
	if (!ledger_read(device, &buffer[0], sizeof buffer, &read, timeout)) {
		return false;
	}

	if (read != sizeof buffer) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		return false;
	}

	struct ledger_transport_reply reply;
	offset += binary_ntoh_uint16(&buffer[offset], &reply.comm_channel_id);
	offset += binary_ntoh_uint8(&buffer[offset], &reply.command_tag);

	switch (reply.command_tag) {
		case LEDGER_TRANSPORT_CMD_GET_VERSION: {
			offset += binary_ntoh_uint32(&buffer[offset], &reply.version.version);
			break;
		}
		case LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL: {
			offset += binary_ntoh_uint16(&buffer[offset], &reply.channel.channel_id);
			break;
		}
		case LEDGER_TRANSPORT_CMD_PING: {
			// ping does not have any data
			break;
		}
		case LEDGER_TRANSPORT_CMD_APDU: {
			offset += binary_ntoh_uint16(&buffer[offset], &reply.apdu_part.sequence_id);
			offset += binary_ntoh(&buffer[offset], &reply.apdu_part.data, sizeof reply.apdu_part.data);
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
		.comm_channel_id = LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID,
		.command_tag = LEDGER_TRANSPORT_CMD_GET_VERSION
	};

	if (!ledger_transport_write(device, &command)) {
		return false;
	}

	struct ledger_transport_reply reply;
	if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT)) {
		return false;
	}

	if (reply.command_tag != LEDGER_TRANSPORT_CMD_GET_VERSION) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
		return false;
	}

	if (version) {
		*version = reply.version.version;
	}
	return true;
}

bool ledger_transport_allocate_channel(struct ledger_device *device, uint16_t *channel_id)
{
	struct ledger_transport_command command = {
		.comm_channel_id = LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID,
		.command_tag = LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL
	};

	if (!ledger_transport_write(device, &command)) {
		return false;
	}

	struct ledger_transport_reply reply;
	if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT)) {
		return false;
	}

	if (reply.command_tag != LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
		return false;
	}

	if (channel_id) {
		*channel_id = reply.channel.channel_id;
	}
	return true;
}

bool ledger_transport_ping(struct ledger_device *device)
{
	struct ledger_transport_command command = {
		.comm_channel_id = LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID,
		.command_tag = LEDGER_TRANSPORT_CMD_PING
	};

	if (!ledger_transport_write(device, &command)) {
		return false;
	}

	struct ledger_transport_reply reply;
	if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT)) {
		return false;
	}

	if (reply.command_tag != LEDGER_TRANSPORT_CMD_PING) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
		return false;
	}
	return true;
}

bool ledger_transport_write_apdu(struct ledger_device *device, uint16_t comm_channel_id, const struct ledger_buffer *buffer)
{
	struct ledger_transport_command command = {
		.comm_channel_id = comm_channel_id,
		.command_tag = LEDGER_TRANSPORT_CMD_APDU,
	};

	size_t offset = 0;
	for (int i = 0; offset < buffer->len; i++) {
		struct ledger_transport_apdu_part apdu_part;
		memset(&apdu_part.data, 0, sizeof apdu_part.data);

		size_t written = 0;
		if (i == 0) {
			written = binary_hton_uint16(&apdu_part.data, buffer->len);
		}

		size_t block_len = sizeof(apdu_part.data) - written;
		if (block_len > (buffer->len - offset)) {
			block_len = buffer->len - offset;
		}

		offset += binary_hton(&apdu_part.data[written], &buffer->data[offset], block_len);

		command.apdu_part = &apdu_part;

		if (!ledger_transport_write(device, &command)) {
			return false;
		}
	}
	return true;
}

bool ledger_transport_read_apdu(struct ledger_device *device, uint16_t comm_channel_id, struct ledger_buffer **buffer)
{
	struct ledger_buffer *_buffer = NULL;

	size_t offset = 0;
	uint16_t sequence_id = 0;

	do {
		struct ledger_transport_reply reply;

		if (!ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT) ) {
			goto err_free_buffer;
		}

		if (reply.comm_channel_id != comm_channel_id) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
			goto err_free_buffer;
		}

		if (reply.command_tag != LEDGER_TRANSPORT_CMD_APDU) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
			goto err_free_buffer;
		}

		if (reply.apdu_part.sequence_id != sequence_id) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_UNEXPECTED_REPLY);
			goto err_free_buffer;
		}

		size_t read = 0;
		if (reply.apdu_part.sequence_id == 0) {
			uint16_t response_data_len = 0;
			read += binary_ntoh_uint16(&reply.apdu_part.data[0], &response_data_len);
			if (response_data_len < 2) {
				LEDGER_SET_ERROR(device, LEDGER_ERROR_INVALID_LENGTH);
				return false;
			}

			_buffer = ledger_buffer_create(response_data_len);
			if (!_buffer) {
				LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
				return false;
			}
		}

		size_t remaining = _buffer->len - offset;
		if (remaining > (sizeof(reply.apdu_part.data) - read)) {
			remaining = sizeof(reply.apdu_part.data) - read;
		}

		offset += binary_ntoh(&reply.apdu_part.data[read], &_buffer->data[offset], remaining);

		sequence_id++;
	} while (offset < _buffer->len);

	*buffer = _buffer;
	return true;

err_free_buffer:
	ledger_buffer_destroy(_buffer);
	return false;
}
