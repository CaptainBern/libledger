#include <stdlib.h>
#include <string.h>

#include "libledger/internal/binary.h"

#include "libledger/error.h"
#include "libledger/transport.h"

int ledger_transport_write(struct ledger_device *device, struct ledger_transport_command *command)
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
		return LEDGER_ERROR_INVALID_COMMAND;
	}

	size_t written = 0;
	int ret = ledger_write(device, &buffer[0], sizeof buffer, &written);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	if (written != sizeof buffer) {
		return LEDGER_ERROR_IO;
	}
	return LEDGER_SUCCESS;
}

int ledger_transport_read(struct ledger_device *device, struct ledger_transport_reply *out, int timeout)
{
	uint8_t buffer[LEDGER_TRANSPORT_PACKET_LENGTH];
	size_t offset = 0;

	size_t read = 0;
	int ret = ledger_read(device, &buffer[0], sizeof buffer, &read, timeout);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	if (read != sizeof buffer) {
		return LEDGER_ERROR_IO;
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
			return LEDGER_ERROR_INVALID_COMMAND;
		}
	}

	if (out) {
		memcpy(out, &reply, sizeof reply);
	}
	return LEDGER_SUCCESS;
}

int ledger_transport_get_version(struct ledger_device *device, uint32_t *version)
{
	struct ledger_transport_command command = {
		.comm_channel_id = LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID,
		.command_tag = LEDGER_TRANSPORT_CMD_GET_VERSION
	};

	int ret = 0;
	ret = ledger_transport_write(device, &command);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	struct ledger_transport_reply reply;

	ret = ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	if (reply.command_tag != LEDGER_TRANSPORT_CMD_GET_VERSION) {
		return LEDGER_ERROR_UNEXPECTED_REPLY;
	}

	if (version) {
		*version = reply.version.version;
	}
	return LEDGER_SUCCESS;
}

int ledger_transport_allocate_channel(struct ledger_device *device, uint16_t *channel_id)
{
	struct ledger_transport_command command = {
		.comm_channel_id = LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID,
		.command_tag = LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL
	};

	int ret = 0;
	ret = ledger_transport_write(device, &command);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	struct ledger_transport_reply reply;
	ret = ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	if (reply.command_tag != LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL) {
		return LEDGER_ERROR_UNEXPECTED_REPLY;
	}

	if (channel_id) {
		*channel_id = reply.channel.channel_id;
	}
	return LEDGER_SUCCESS;
}

int ledger_transport_ping(struct ledger_device *device)
{
	struct ledger_transport_command command = {
		.comm_channel_id = LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID,
		.command_tag = LEDGER_TRANSPORT_CMD_PING
	};

	int ret = 0;
	ret = ledger_transport_write(device, &command);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	struct ledger_transport_reply reply;
	ret = ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	if (reply.command_tag != LEDGER_TRANSPORT_CMD_PING) {
		return LEDGER_ERROR_UNEXPECTED_REPLY;
	}
	return LEDGER_SUCCESS;
}

int ledger_transport_write_apdu(struct ledger_device *device, uint16_t comm_channel_id, const uint8_t *data, size_t data_len)
{
	struct ledger_transport_command command = {
		.comm_channel_id = comm_channel_id,
		.command_tag = LEDGER_TRANSPORT_CMD_APDU,
	};

	size_t offset = 0;
	for (int i = 0; offset < data_len; i++) {
		struct ledger_transport_apdu_part apdu_part;
		memset(&apdu_part.data, 0, sizeof apdu_part.data);

		size_t written = 0;
		if (i == 0) {
			written = binary_hton_uint16(&apdu_part.data, data_len);
		}

		size_t block_len = sizeof(apdu_part.data) - written;
		if (block_len > (data_len - offset)) {
			block_len = data_len - offset;
		}

		offset += binary_hton(&apdu_part.data[written], &data[offset], block_len);

		command.apdu_part = &apdu_part;

		int ret = ledger_transport_write(device, &command);
		if (ret != LEDGER_SUCCESS) {
			return ret;
		}
	}
	return LEDGER_SUCCESS;
}

int ledger_transport_read_apdu(struct ledger_device *device, uint16_t comm_channel_id, struct ledger_transport_apdu **apdu)
{
	int ret = 0;

	struct ledger_transport_apdu *_apdu = NULL;

	size_t offset = 0;
	uint16_t sequence_id = 0;

	do {
		struct ledger_transport_reply reply;

		ret = ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT);
		if (ret != LEDGER_SUCCESS) {
			goto err_free_apdu;
		}

		if (reply.comm_channel_id != comm_channel_id) {
			ret = LEDGER_ERROR_UNEXPECTED_REPLY;
			goto err_free_apdu;
		}

		if (reply.command_tag != LEDGER_TRANSPORT_CMD_APDU) {
			ret = LEDGER_ERROR_UNEXPECTED_REPLY;
			goto err_free_apdu;
		}

		if (reply.apdu_part.sequence_id != sequence_id) {
			ret =  LEDGER_ERROR_UNEXPECTED_REPLY;
			goto err_free_apdu;
		}

		size_t read = 0;
		if (reply.apdu_part.sequence_id == 0) {
			uint16_t response_data_len = 0;
			read += binary_ntoh_uint16(&reply.apdu_part.data[0], &response_data_len);
			if (response_data_len < 2) {
				return LEDGER_ERROR_INCORRECT_LENGTH;
			}

			_apdu = malloc(sizeof(struct ledger_transport_apdu) + response_data_len);
			if (_apdu) {
				_apdu->len = response_data_len;
			} else {
				return LEDGER_ERROR_NOMEM;
			}
		}

		size_t remaining = _apdu->len - offset;
		if (remaining > (sizeof(reply.apdu_part.data) - read)) {
			remaining = sizeof(reply.apdu_part.data) - read;
		}

		offset += binary_ntoh(&reply.apdu_part.data[read], &_apdu->data[offset], remaining);

		sequence_id++;
	} while (offset < _apdu->len);

	*apdu = _apdu;
	return LEDGER_SUCCESS;

err_free_apdu:
	ledger_transport_apdu_free(_apdu);
	return ret;
}

void ledger_transport_apdu_free(struct ledger_transport_apdu *apdu)
{
	if (apdu) {
		free(apdu);
	}
}
