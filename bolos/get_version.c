#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "libledger/apdu.h"
#include "libledger/error.h"
#include "libledger/transport.h"

#include "internal/binary.h"

#include "libledger/bolos/apdu.h"
#include "libledger/bolos/get_version.h"

static inline bool check_len(size_t len, size_t offset, size_t required)
{
	return (len - offset) > required;
}

bool read_length(const uint8_t *buffer, size_t buffer_len, size_t *offset, size_t *length)
{
	if (!check_len(buffer_len, *offset, 1)) {
		return false;
	}

	uint8_t _length = 0;
	*offset += binary_ntoh_uint8(&buffer[*offset], &_length);
	*length = _length;
	return true;
}

bool read_string(const uint8_t *buffer, size_t buffer_len, size_t *offset, char **string)
{
	size_t _string_len = 0;
	if (!read_length(buffer, buffer_len, offset, &_string_len)) {
		return false;
	}

	char *_string = calloc(_string_len, sizeof(char));
	if (!_string) {
		return false;
	}

	*offset += binary_ntoh(&buffer[*offset], _string, _string_len);
	*string = _string;
	return true;
}

bool read_array(const uint8_t *buffer, size_t buffer_len, size_t *offset, uint8_t **array, size_t *array_len)
{
	size_t _array_len = 0;
	if (!read_length(buffer, buffer_len, offset, &_array_len)) {
		return false;
	}

	uint8_t *_array = calloc(_array_len, sizeof(uint8_t));
	if (!_array) {
		return false;
	}

	*offset += binary_ntoh(&buffer[*offset], _array, _array_len);
	*array_len = _array_len;
	*array = _array;
	return true;
}

struct ledger_bolos_version *read_version(const uint8_t *buffer, size_t buffer_len)
{
	uint32_t target_id = 0;

	char *os_version = NULL;

	size_t flags_len = 0;
	uint8_t *flags = NULL;

	char *mcu_version = NULL;

	size_t mcu_hash_len = 0;
	uint8_t *mcu_hash = NULL;

	size_t offset = 0;

	// Read the target ID
	if (!check_len(buffer_len, offset, 4)) {
		return NULL;
	}

	offset += binary_ntoh_uint32(&buffer[offset], &target_id);

	// Read the OS version
	if (!read_string(buffer, buffer_len, &offset, &os_version)) {
		return NULL;
	}

	// Read the flags
	if (!read_array(buffer, buffer_len, &offset, &flags, &flags_len)) {
		goto err_free_os_version;
	}

	// Read the MCU version
	if (!read_string(buffer, buffer_len, &offset, &mcu_version)) {
		goto err_free_flags;
	}

	// Read the MCU hash
	if (!read_array(buffer, buffer_len, &offset, &mcu_hash, &mcu_hash_len)) {
		goto err_free_mcu_version;
	}

	// Check SW
	if ((buffer_len - offset) != 2) {
		goto err_free_mcu_hash;
	}

	struct ledger_bolos_version *version = malloc(sizeof(struct ledger_bolos_version));
	if (!version) {
		goto err_free_mcu_hash;
	}

	version->target_id = target_id;
	version->os_version = os_version;
	version->flags.flags_len = flags_len;
	version->flags.flags = flags;
	version->mcu_version = mcu_version;
	version->mcu_hash.hash_len = mcu_hash_len;
	version->mcu_hash.hash = mcu_hash;
	return version;

err_free_mcu_hash:
	free(mcu_hash);
err_free_mcu_version:
	free(mcu_version);
err_free_flags:
	free(flags);
err_free_os_version:
	free(os_version);
	return NULL;
}

struct ledger_bolos_version *ledger_bolos_get_version(
		struct ledger_device *device, uint16_t comm_channel_id)
{
	uint8_t apdu[LEDGER_APDU_HEADER_LENGTH];
	apdu[LEDGER_APDU_OFFSET_CLA] = LEDGER_BOLOS_APDU_CLA;
	apdu[LEDGER_APDU_OFFSET_INS] = LEDGER_BOLOS_APDU_INS_GET_VERSION;
	apdu[LEDGER_APDU_OFFSET_P1] = 0x00;
	apdu[LEDGER_APDU_OFFSET_P2] = 0x00;
	apdu[LEDGER_APDU_OFFSET_LC] = 0x00;

	if (ledger_transport_write_apdu(device,
			comm_channel_id, &apdu[0], sizeof apdu) != LEDGER_SUCCESS) {
		return NULL;
	}

	struct ledger_transport_apdu *reply = NULL;
	if (ledger_transport_read_apdu(device,
			comm_channel_id, &reply) != LEDGER_SUCCESS) {
		return NULL;
	}

	if (!ledger_apdu_ok(reply->data, reply->len)) {
		goto err_free_reply;
	}

	struct ledger_bolos_version *version = read_version(reply->data, reply->len);
	ledger_transport_apdu_free(reply);
	return version;

err_free_reply:
	ledger_transport_apdu_free(reply);
	return NULL;
}

void ledger_bolos_free_version(struct ledger_bolos_version *version) {
	if (version) {
		free(version->os_version);
		free(version->flags.flags);
		free(version->mcu_version);
		free(version->mcu_hash.hash);
		free(version);
	}
}
