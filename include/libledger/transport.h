#ifndef LIBLEDGER_TRANSPORT_H_
#define LIBLEDGER_TRANSPORT_H_

#include <stddef.h>
#include <stdint.h>

#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDGER_TRANSPORT_DEFAULT_TIMEOUT         3500
#define LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID 0x0101

#define LEDGER_TRANSPORT_PACKET_LENGTH        0x40
#define LEDGER_TRANSPORT_CMD_GET_VERSION      0x00
#define LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL 0x01
#define LEDGER_TRANSPORT_CMD_PING             0x02
#define LEDGER_TRANSPORT_CMD_APDU             0x05

struct ledger_transport_apdu {
	size_t len;
	uint8_t data[0];
};

struct ledger_transport_apdu_part {
	uint16_t sequence_id;
	uint8_t data[LEDGER_TRANSPORT_PACKET_LENGTH - 5];
};

struct ledger_transport_command {
    uint16_t comm_channel_id;
    uint8_t command_tag;

    union {
        struct ledger_transport_apdu_part *apdu_part;
    };
};

struct ledger_transport_reply {
    uint16_t comm_channel_id;
    uint8_t command_tag;

    union {
		struct {
			uint32_t version;
		} version;

		struct {
			uint16_t channel_id;
		} channel;

		struct ledger_transport_apdu_part apdu_part;
    };
};

extern int ledger_transport_write(struct ledger_device *device,
		struct ledger_transport_command *command);

extern int ledger_transport_read(struct ledger_device *device,
		struct ledger_transport_reply *out, int timeout);

extern int ledger_transport_get_version(struct ledger_device *device,
		uint32_t *version);

extern int ledger_transport_allocate_channel(struct ledger_device *device,
		uint16_t *channel_id);

extern int ledger_transport_ping(struct ledger_device *device);

extern int ledger_transport_write_apdu(struct ledger_device *device,
		uint16_t comm_channel_id, const uint8_t *data, size_t data_len);

extern int ledger_transport_read_apdu(struct ledger_device *device,
		uint16_t comm_channel_id, struct ledger_transport_apdu **apdu);

extern void ledger_transport_apdu_free(struct ledger_transport_apdu *apdu);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_TRANSPORT_H_ */
