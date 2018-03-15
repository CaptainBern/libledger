#ifndef LIBLEDGER_TRANSPORT_H_
#define LIBLEDGER_TRANSPORT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "libledger/buffer.h"
#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The default timeout used by the ledger transport functions */
#define LEDGER_TRANSPORT_DEFAULT_TIMEOUT         3500

/* Default channel ID used by Ledger devices. */
#define LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID 0x0101

/* Transport message length. */
#define LEDGER_TRANSPORT_MESSAGE_LENGTH 0x40

/* Length of the transport message header */
#define LEDGER_TRANSPORT_HEADER_LENGTH 0x03

/* APDU message header length */
#define LEDGER_TRANSPORT_APDU_HEADER_LENGTH (0x02 + LEDGER_TRANSPORT_HEADER_LENGTH)

/* APDU message data length */
#define LEDGER_TRANSPORT_APDU_DATA_LEN      (LEDGER_TRANSPORT_MESSAGE_LENGTH - LEDGER_TRANSPORT_APDU_HEADER_LENGTH)

#define LEDGER_TRANSPORT_CMD_GET_VERSION      0x00
#define LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL 0x01
#define LEDGER_TRANSPORT_CMD_PING             0x02
#define LEDGER_TRANSPORT_CMD_APDU             0x05

/**
 * struct ledger_transport_header - Ledger transport message header.
 * @comm_channel_id: The communication channel ID over which the
 *                   message should be sent.
 * @command_tag: The message command tag.
 *
 * A common header shared between transport commands and replies.
 * Be aware that the channel ID is only used for APDU commands.
 */
struct ledger_transport_header {
	uint16_t comm_channel_id;
	uint8_t command_tag;
};

/**
 * struct ledger_transport_apdu - Transport APDU payload data.
 * @sequence_id: The payload sequence ID.
 * @data: The payload data.
 *
 * Used to exchange APDUs with the device. When the sequence ID
 * is 0, the first two bytes in data are the total length of the
 * APDU.
 */
struct ledger_transport_apdu {
	uint16_t sequence_id;
	uint8_t data[LEDGER_TRANSPORT_APDU_DATA_LEN];
};

/**
 * struct ledger_transport_command - A transport command.
 * @header: Command header.
 * @payload.apdu: APDU command payload.
 *
 * Used to send a command to the device. The protocol only
 * supports payloads to be sent to the device for APDU messages.
 */
struct ledger_transport_command {
	struct ledger_transport_header header;

    union {
        struct ledger_transport_apdu apdu;
    } payload;
};

/**
 * struct ledger_transport_reply - A transport reply.
 * @header: Reply header.
 * @payload.version: Version replied from a GET_VERSION command.
 * @payload.channel_id: Channel ID replied from an ALLOCATE_CHANNEL command.
 * @payload.apdu: An APDU replied from an APDU command.
 *
 * A reply received from the device. The value of the payload
 * depends on the command tag in the header.
 */
struct ledger_transport_reply {
    struct ledger_transport_header header;

    union {
		uint32_t version;
		uint16_t channel_id;
		struct ledger_transport_apdu apdu;
    } payload;
};

/**
 * ledger_transport_write() - Write a transport command to the device.
 * @device: A ledger_device pointer returned from ledger_open().
 * @command: The command to send to the device.
 *
 * Write raw transport commands to the device.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_transport_write(struct ledger_device *device, struct ledger_transport_command *command);

/**
 * ledger_transport_read() - Read a transport reply from the device.
 * @device: A ledger_device pointer returned from ledger_open().
 * @reply: Pointer to where the reply.
 * @timeout: Timeout in milliseconds or -1 for blocking wait.
 *
 * Read raw transport replies from the device.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_transport_read(struct ledger_device *device, struct ledger_transport_reply *out, int timeout);

/**
 * ledger_transport_get_version() - Get the transport protocol version.
 * @device: A ledger_device pointer returned from ledger_open().
 * @version: Pointer to the location where the version should be stored.
 *
 * Retrieve the ledger transport protocol version from the device.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_transport_get_version(struct ledger_device *device, uint32_t *version);

/**
 * ledger_transport_allocate_channel() - Allocate a communication channel.
 * @device: A ledger_device pointer returned from ledger_open().
 * @channel_id: Pointer to the location where the allocated channel ID should be stored.
 *
 * Allocate a communication channel which can be used to write and read APDUs.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_transport_allocate_channel(struct ledger_device *device, uint16_t *channel_id);

/**
 * ledger_transport_ping() - Ping the device.
 * @device: A ledger_device pointer returned from ledger_open().
 *
 * Ping the device in order to check the data link. This function is also
 * useful to verify whether or not the device is a Ledger device (combined
 * with ledger_transport_get_version()).
 *
 * Return: True on success or false on failure (or when no pong message is received).
 */
extern bool ledger_transport_ping(struct ledger_device *device);

/**
 * ledger_transport_write_apdu() - Write an APDU to the device.
 * @device: A ledger_device pointer returned from ledger_open().
 * @channel_id: The channel ID.
 * @apdu: The APDU to send.
 *
 * Write APDUs to the device. The function will take care of properly splitting up
 * the APDU in chunks in case it is too big for a single transport message.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_transport_write_apdu(struct ledger_device *device, uint16_t channel_id, const struct ledger_buffer *apdu);

/**
 * ledger_transport_read_apdu() - Read an APDU from the device.
 * @device: A ledger_device pointer returned from ledger_open().
 * @channel_id: The channel ID.
 * @apdu: Pointer to the location where the APDU should be stored.
 *        ledger_buffer_destroy() should be called on the apdu after
 *        you are done using it.
 *
 * Read APDUs from the device. The function will take care of reconstructing the
 * APDU into a single buffer in case it was too big to fit in a single
 * transport message.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_transport_read_apdu(struct ledger_device *device, uint16_t channel_id, struct ledger_buffer **apdu);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_TRANSPORT_H_ */
