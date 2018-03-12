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

#define LEDGER_TRANSPORT_DEFAULT_TIMEOUT         3500
#define LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID 0x0101

#define LEDGER_TRANSPORT_PACKET_LENGTH 0x40
#define LEDGER_TRANSPORT_HEADER_LENGTH 0x03

#define LEDGER_TRANSPORT_APDU_HEADER_LENGTH (0x02 + LEDGER_TRANSPORT_HEADER_LENGTH)
#define LEDGER_TRANSPORT_APDU_DATA_LEN      (LEDGER_TRANSPORT_PACKET_LENGTH - LEDGER_TRANSPORT_APDU_HEADER_LENGTH)

#define LEDGER_TRANSPORT_CMD_GET_VERSION      0x00
#define LEDGER_TRANSPORT_CMD_ALLOCATE_CHANNEL 0x01
#define LEDGER_TRANSPORT_CMD_PING             0x02
#define LEDGER_TRANSPORT_CMD_APDU             0x05

struct ledger_transport_header {
	uint16_t comm_channel_id;
	uint8_t command_tag;
};

struct ledger_transport_apdu {
	uint16_t sequence_id;
	uint8_t data[LEDGER_TRANSPORT_APDU_DATA_LEN];
};

struct ledger_transport_command {
	struct ledger_transport_header header;

    union {
        struct ledger_transport_apdu apdu;
    } payload;
};

struct ledger_transport_reply {
    struct ledger_transport_header header;

    union {
		uint32_t version;
		uint16_t channel_id;
		struct ledger_transport_apdu apdu;
    } payload;
};

extern bool ledger_transport_write(struct ledger_device *device, struct ledger_transport_command *command);

extern bool ledger_transport_read(struct ledger_device *device, struct ledger_transport_reply *out, int timeout);

extern bool ledger_transport_get_version(struct ledger_device *device, uint32_t *version);

extern bool ledger_transport_allocate_channel(struct ledger_device *device, uint16_t *channel_id);

extern bool ledger_transport_ping(struct ledger_device *device);

extern bool ledger_transport_write_apdu(struct ledger_device *device, uint16_t comm_channel_id, const struct ledger_buffer *adpu);

extern bool ledger_transport_read_apdu(struct ledger_device *device, uint16_t comm_channel_id, struct ledger_buffer **adpu);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_TRANSPORT_H_ */
