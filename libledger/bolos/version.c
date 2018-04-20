#include <stdlib.h>

#include "internal/cursor.h"
#include "internal/macros.h"

#include "libledger/error.h"
#include "libledger/apdu.h"
#include "libledger/bolos/constants.h"

#include "libledger/bolos/version.h"

#define LEDGER_BOLOS_VERSION_DATA_LENGTH 64

void ledger_bolos_version_destroy(struct ledger_bolos_version *version)
{
	if (version) {
		if (version->os_version)
			free(version->os_version);

		if (version->flags.flags)
			free(version->flags.flags);

		if (version->mcu_version)
			free(version->mcu_version);

		if (version->mcu_hash.hash)
			free(version->mcu_hash.hash);

		free(version);
	}
}

bool ledger_bolos_get_version(struct ledger_device *device, uint16_t channel_id, struct ledger_bolos_version **version)
{
	struct ledger_apdu_command command = LEDGER_APDU_COMMAND_INITIALIZER(
			LEDGER_BOLOS_APDU_CLA, LEDGER_BOLOS_APDU_INS_VERSION, 0x0, 0x0, NULL, 0);

	struct ledger_cursor cursor;
	size_t data_len = 0;
	uint8_t data[LEDGER_BOLOS_VERSION_DATA_LENGTH];
	uint16_t status = 0;

	if (!ledger_apdu_exchange(device, channel_id, &command, &data_len, data, sizeof(data), &status))
		return false;

	if (status != 0x9000) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_OTHER);
		return false;
	}

	ledger_cursor_init(&cursor, data, data_len);

	struct ledger_bolos_version *_version = calloc(1, sizeof(struct ledger_bolos_version));
	if (!_version) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
		return false;
	}

	if (!ledger_cursor_read_u32(&cursor, &_version->target_id)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_version;
	}

	uint8_t os_version_len = 0;
	if (!ledger_cursor_read_u8(&cursor, &os_version_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_version;
	}

	_version->os_version = calloc(os_version_len, sizeof(char));
	if (!_version->os_version) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
		goto err_destroy_version;
	}

	if (!ledger_cursor_read_bytes(&cursor, (uint8_t *) _version->os_version, os_version_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_version;
	}

	if (!ledger_cursor_read_u8(&cursor, &_version->flags.len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_version;
	}

	_version->flags.flags = calloc(_version->flags.len, sizeof(uint8_t));
	if (!_version->flags.flags) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
		goto err_destroy_version;
	}

	if (!ledger_cursor_read_bytes(&cursor, _version->flags.flags, _version->flags.len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_version;
	}

	uint8_t mcu_version_len = 0;
	if (!ledger_cursor_read_u8(&cursor, &mcu_version_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_version;
	}

	_version->mcu_version = calloc(mcu_version_len, sizeof(char));
	if (!_version->mcu_version) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
		goto err_destroy_version;
	}

	if (!ledger_cursor_read_bytes(&cursor, (uint8_t *) _version->mcu_version, mcu_version_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_version;
	}

	// Only target ID 0x31100003 sends the MCU hash
	if ((_version->target_id & 0xff) == 0x03) {
		if (!ledger_cursor_read_u8(&cursor, &_version->mcu_hash.len)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
			goto err_destroy_version;
		}

		_version->mcu_hash.hash = calloc(_version->mcu_hash.len, sizeof(uint8_t));
		if (!_version->mcu_hash.hash) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
			goto err_destroy_version;
		}

		if (!ledger_cursor_read_bytes(&cursor, _version->mcu_hash.hash, _version->mcu_hash.len)) {
			LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
			goto err_destroy_version;
		}
	}

	*version = _version;

	return true;

err_destroy_version:
	ledger_bolos_version_destroy(_version);
	return false;
}
