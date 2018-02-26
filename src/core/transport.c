#include <stdlib.h>
#include <string.h>

#include "libledger/internal/binary.h"

#include "libledger/core/error.h"
#include "libledger/core/transport.h"

int ledger_transport_write(struct ledger_device *device, struct ledger_transport_command *command) {
    uint8_t buffer[LEDGER_TRANSPORT_PACKET_SIZE];
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
        default: {
            return LEDGER_ERROR_TRANSPORT_UNKNOWN_COMMAND;
        }
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

int ledger_transport_read(struct ledger_device *device, struct ledger_transport_reply *out, int timeout) {
    uint8_t buffer[LEDGER_TRANSPORT_PACKET_SIZE];
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
            return LEDGER_ERROR_TRANSPORT_UNKNOWN_COMMAND;
        } 
    }

    if (out) {
        memcpy(out, &reply, sizeof reply);
    }
    return LEDGER_SUCCESS;
}

int ledger_transport_get_version(struct ledger_device *device, struct ledger_transport_version *version) {
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
        return LEDGER_ERROR_TRANSPORT_UNEXPECTED_REPLY;
    }

    version->version = reply.version.version;
    return LEDGER_SUCCESS;
}

int ledger_transport_allocate_channel(struct ledger_device *device, struct ledger_transport_channel *channel) {
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
        return LEDGER_ERROR_TRANSPORT_UNEXPECTED_REPLY;
    }
    
    channel->channel_id = reply.channel.channel_id;
    return LEDGER_SUCCESS;
}

int ledger_transport_ping(struct ledger_device *device) {
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
        return LEDGER_ERROR_TRANSPORT_UNEXPECTED_REPLY;
    }
    return LEDGER_SUCCESS;
}

int ledger_transport_write_apdu(struct ledger_device *device, uint16_t comm_channel_id, const uint8_t *apdu, size_t apdu_len) {    
    struct ledger_transport_command command = {
        .comm_channel_id = comm_channel_id,
        .command_tag = LEDGER_TRANSPORT_CMD_APDU,
    };

    size_t offset = 0;
    for (int i = 0; offset < apdu_len; i++) {
        struct ledger_transport_apdu_part apdu_part;
        memset(&apdu_part.data, 0, sizeof apdu_part.data);
        
        size_t written = 0;
        if (i == 0) {
            written = binary_hton_uint16(&apdu_part.data, apdu_len);
        }

        size_t block_len = sizeof(apdu_part.data) - written;
        if (block_len > (apdu_len - offset)) {
            block_len = apdu_len - offset;
        }

        offset += binary_hton(&apdu_part.data[written], &apdu[offset], block_len);

        command.apdu_part = &apdu_part;

        int ret = ledger_transport_write(device, &command);
        if (ret != LEDGER_SUCCESS) {
            return ret;
       }
    }

    return LEDGER_SUCCESS;
}

int ledger_transport_read_apdu(struct ledger_device *device, uint16_t comm_channel_id, struct ledger_transport_apdu_reply **reply) {
    int ret = 0;

    uint8_t *apdu;
    uint16_t apdu_len = 0;

    size_t offset = 0;
    uint16_t sequence_id = 0;

    do {
        struct ledger_transport_reply reply;
        
        ret = ledger_transport_read(device, &reply, LEDGER_TRANSPORT_DEFAULT_TIMEOUT);
        if (ret != LEDGER_SUCCESS) {
            goto error;
        }

        if (reply.comm_channel_id != comm_channel_id || reply.command_tag != LEDGER_TRANSPORT_CMD_APDU) {
            ret = LEDGER_ERROR_TRANSPORT_UNEXPECTED_REPLY;
            goto error;
        }

        if (reply.apdu_part.sequence_id != sequence_id) {
            ret =  LEDGER_ERROR_TRANSPORT_UNEXPECTED_REPLY;
            goto error;
        }

        size_t read = 0;
        if (reply.apdu_part.sequence_id == 0) {
            // Typically apdu_len is at least 2. It's up to the user of this library to make sure of that though
            read += binary_ntoh_uint16(&reply.apdu_part.data[0], &apdu_len);
            apdu = malloc(apdu_len * sizeof(uint8_t));
            if (!apdu) {
                return LEDGER_ERROR_NOMEM;
            }
        }

        size_t remaining = apdu_len - offset;
        if (remaining > (sizeof(reply.apdu_part.data) - read)) {
            remaining = sizeof(reply.apdu_part.data) - read;
        }

        offset += binary_ntoh(&reply.apdu_part.data[read], &apdu[offset], remaining);
        sequence_id++;
    } while (offset < apdu_len);

    struct ledger_transport_apdu_reply *_reply = malloc(sizeof(struct ledger_transport_apdu_reply));
    if (!_reply) {
        ret = LEDGER_ERROR_NOMEM;
        goto error;
    }

    _reply->apdu = apdu;
    _reply->apdu_len = apdu_len;

    *reply = _reply;
    return LEDGER_SUCCESS;

error:
    if (apdu) {
        free(apdu);
    }
    return ret;
}

void ledger_transport_free_apdu_reply(struct ledger_transport_apdu_reply *reply) {
    if (reply) {
        free(reply->apdu);
        free(reply);
    }
}