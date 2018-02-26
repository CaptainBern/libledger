#ifndef LIBLEDGER_TRANSPORT_H_
#define LIBLEDGER_TRANSPORT_H_

#include <stddef.h>
#include <stdint.h>

#include "libledger/core/device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDGER_TRANSPORT_DEFAULT_TIMEOUT         3500
#define LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID 0x0101

#define LEDGER_TRANSPORT_PACKET_SIZE          0x40
#define LEDGER_TRANSPORT_CMD_GET_VERSION      0x00
#define LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL 0x01
#define LEDGER_TRANSPORT_CMD_PING             0x02
#define LEDGER_TRANSPORT_CMD_APDU             0x05

struct ledger_transport_version {
    uint32_t version;
};

struct ledger_transport_channel {
    uint16_t channel_id;
};

struct ledger_transport_apdu_part {
    uint16_t sequence_id;
    uint8_t data[LEDGER_TRANSPORT_PACKET_SIZE - 5]; // 5: comm_channel_id + command_tag + sequence_id
};

struct ledger_transport_command {
    uint16_t comm_channel_id; // Only used for APDU commands, but should be set to LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID for other commands
    uint8_t command_tag;

    union {
        struct ledger_transport_apdu_part *apdu_part;
    };
};

struct ledger_transport_reply {
    uint16_t comm_channel_id;
    uint8_t command_tag;

    union {
        struct ledger_transport_version version;
        struct ledger_transport_channel channel;
        struct ledger_transport_apdu_part apdu_part;
    };
};

struct ledger_transport_apdu_reply {
    uint8_t *apdu;
    uint16_t apdu_len;
};

extern int ledger_transport_write(struct ledger_device *device, struct ledger_transport_command *command);

extern int ledger_transport_read(struct ledger_device *device, struct ledger_transport_reply *out, int timeout);

extern int ledger_transport_get_version(struct ledger_device *device, struct ledger_transport_version *version);

extern int ledger_transport_allocate_channel(struct ledger_device *device, struct ledger_transport_channel *channel);

extern int ledger_transport_ping(struct ledger_device *device);

extern int ledger_transport_write_apdu(struct ledger_device *device, uint16_t comm_channel_id, const uint8_t *apdu, size_t apdu_len);

extern int ledger_transport_read_apdu(struct ledger_device *device, uint16_t comm_channel_id, struct ledger_transport_apdu_reply **reply);

extern void ledger_transport_free_apdu_reply(struct ledger_transport_apdu_reply *reply);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_TRANSPORT_H_ */