#ifndef LIBLEDGER_APDU_H_
#define LIBLEDGER_APDU_H_

#include <stdbool.h>
#include <stdint.h>

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
 * @data_len: APDU data length.
 *
 * A Simple utility macro to initialize APDU commands.
 */
#define LEDGER_APDU_COMMAND_INITIALIZER(cla, ins, p1, p2, data, data_len) { (cla), (ins), (p1), (p2), (data), (data_len) }

/**
 * struct ledger_apdu_command - An APDU command packet.
 * @cla: Class byte.
 * @ins: Instruction byte.
 * @p1: First parameter byte.
 * @p2: Second parameter byte.
 * @data: APDU data.
 * @len: Length of the APDU data.
 *
 * A Ledger APDU command. Used to communicate
 * with Ledger devices.
 */
struct ledger_apdu_command {
	uint8_t cla;
	uint8_t ins;
	uint8_t p1;
	uint8_t p2;
	uint8_t *data;
	size_t data_len;
};

/**
 * ledger_apdu_write() - Write an APDU command to the device.
 * @device: A ledger_device pointer returned from ledger_open().
 * @channel_id: The communication channel ID to use.
 * @command: The command to send to the device.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_apdu_write(struct ledger_device *device, uint16_t channel_id, struct ledger_apdu_command *command);

/**
 * ledger_apdu_read() - Read an APDU from the device.
 * @device: A ledger_device pointer returned from ledger_open().
 * @channel_id: The communication channel ID to use.
 * @len: Length of the actual APDU reply data.
 * @buffer: Buffer for the APDU reply data.
 * @buffer_len: Length of the reply data buffer.
 * @status: The status code from the APDU.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_apdu_read(struct ledger_device *device, uint16_t channel_id, size_t *len, uint8_t *buffer, size_t buffer_len, uint16_t *status);

/**
 * ledger_apdu_exchange() - Exchange an APDU with the device.
 * @device: A ledger_device pointer returned from ledger_open().
 * @channel_id: The communication channel ID to use.
 * @command: The command to send to the device.
  * @len: Length of the actual APDU reply data.
 * @buffer: Buffer for the APDU reply data.
 * @buffer_len: Length of the reply data buffer.
 * @status: The status code from the APDU.
 *
 * This function first calls ledger_apdu_write() and
 * then ledger_apdu_read(). Just to save you a few lines
 * of code.
 *
 * Return: True on success or false on failure.
 */
extern bool ledger_apdu_exchange(struct ledger_device *device, uint16_t channel_id, struct ledger_apdu_command *command, size_t *len, uint8_t *buffer, size_t buffer_len, uint16_t *status);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_APDU_H_ */
