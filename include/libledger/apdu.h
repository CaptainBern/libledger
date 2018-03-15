#ifndef LIBLEDGER_APDU_H_
#define LIBLEDGER_APDU_H_

#include <stdbool.h>
#include <stdint.h>

#include "libledger/buffer.h"
#include "libledger/device.h"

#ifdef __cplusplus
extern "C" {
#endif

/* APDU header length */
#define LEDGER_APDU_HEADER_LENGTH   0x05

/* Maximum APDU data length */
#define LEDGER_APDU_MAX_DATA_LENGTH 0xff

/**
 * LEDGER_APDU_COMMAND_INITIALIZER() - Initialize an APDU command.
 * @cla: Class byte.
 * @ins: Instruction byte.
 * @p1: First parameter byte.
 * @p2: Second parameter byte.
 * @data: APDU data.
 *
 * A Simple utility macro to initialize APDU commands.
 */
#define LEDGER_APDU_COMMAND_INITIALIZER(cla, ins, p1, p2, data) { (cla), (ins), (p1), (p2), (data) }

/**
 * struct ledger_apdu_command - An APDU command packet.
 * @cla: Class byte.
 * @ins: Instruction byte.
 * @p1: First parameter byte.
 * @p2: Second parameter byte.
 * @data: APDU data, may be set to NULL.
 *
 * A Ledger APDU command. Used to communicate
 * with Ledger devices.
 */
struct ledger_apdu_command {
	uint8_t cla;
	uint8_t ins;
	uint8_t p1;
	uint8_t p2;
	struct ledger_buffer *data;
};

/**
 * struct ledger_apdu_reply - An APDU reply packet.
 * @data: Reply data, may be set to NULL.
 * @status: APDU Status code.
 *
 * A Ledger APDU reply. The status field is always set.
 */
struct ledger_apdu_reply {
	struct ledger_buffer *data;
	uint16_t status;
};

/**
 * ledger_apdu_reply_destroy() - Destroy an APDU reply.
 * @reply: A ledger_apdu_reply pointer set by the
 *         ledger_apdu_exchange funtion.
 */
extern void ledger_apdu_reply_destroy(struct ledger_apdu_reply *reply);

/**
 * ledger_apdu_exchange() - Exchange an APDU with the device.
 * @device: A ledger_device pointer returned from ledger_open().
 * @channel_id: The communication channel ID to use.
 * @command: The command to send to the device.
 * @reply: Pointer location to the ledger_apdu_reply.
 *
 * This function will write the command to the device and then
 * try to read the reply. The reply will be allocated on the heap
 * and should be destroyed using ledger_apdu_reply_destroy after
 * you are done using it.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_apdu_exchange(struct ledger_device *device, uint16_t channel_id, struct ledger_apdu_command *command, struct ledger_apdu_reply **reply);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_APDU_H_ */
