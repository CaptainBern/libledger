#ifndef LIBLEDGER_APDU_H_
#define LIBLEDGER_APDU_H_

#include <stdbool.h>
#include <stdint.h>

#include "libledger/buffer.h"
#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDGER_APDU_HEADER_LENGTH   0x05
#define LEDGER_APDU_MAX_DATA_LENGTH 0xff

#define LEDGER_APDU_COMMAND_INITIALIZER(cla, ins, p1, p2, data) { (cla), (ins), (p1), (p2), (data) }

struct ledger_apdu_command {
	uint8_t cla;
	uint8_t ins;
	uint8_t p1;
	uint8_t p2;
	struct ledger_buffer *data;
};

struct ledger_apdu_reply {
	struct ledger_buffer *data;
	uint16_t status;
};

extern void ledger_apdu_reply_destroy(struct ledger_apdu_reply *reply);

extern bool ledger_apdu_exchange(struct ledger_device *device, uint16_t channel_id, struct ledger_apdu_command *command, struct ledger_apdu_reply **reply);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_APDU_H_ */
