#include <stdlib.h>

#include "internal/cursor.h"
#include "internal/macros.h"
#include "libledger/error.h"
#include "libledger/apdu.h"
#include "libledger/bolos/apdu.h"

#include "libledger/bolos/bolos.h"

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
			LEDGER_BOLOS_APDU_CLA, LEDGER_BOLOS_APDU_INS_GET_VERSION, 0x00, 0x00, NULL);

	struct ledger_apdu_reply *reply = NULL;
	if (!ledger_apdu_exchange(device, channel_id, &command, &reply))
		return false;

	if (reply->status != 0x9000)
		goto err_destroy_apdu_reply;

	struct ledger_cursor cursor;
	ledger_cursor_init(&cursor, reply->data->data, reply->data->len);

	uint32_t target_id = 0;
	if (!ledger_cursor_read_u32(&cursor, &target_id)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_apdu_reply;
	}

	uint8_t os_version_len = 0;
	if (!ledger_cursor_read_u8(&cursor, &os_version_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_apdu_reply;
	}

	char *os_version = calloc(os_version_len, sizeof(char));
	if (!os_version) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
		goto err_destroy_apdu_reply;
	}

	if (!ledger_cursor_read_bytes(&cursor, (uint8_t *) os_version, os_version_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_os_version;
	}

	uint8_t flags_len = 0;
	if (!ledger_cursor_read_u8(&cursor, &flags_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_os_version;
	}

	uint8_t *flags = calloc(flags_len, sizeof(uint8_t));
	if (!flags) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
		goto err_destroy_os_version;
	}

	if (!ledger_cursor_read_bytes(&cursor, flags, flags_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_flags;
	}

	uint8_t mcu_version_len = 0;
	if (!ledger_cursor_read_u8(&cursor, &mcu_version_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_flags;
	}

	char *mcu_version = calloc(mcu_version_len, sizeof(char));
	if (!mcu_version) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
		goto err_destroy_flags;
	}

	if (!ledger_cursor_read_bytes(&cursor, (uint8_t *) mcu_version, mcu_version_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_mcu_version;
	}

	uint8_t mcu_hash_len = 0;
	if (!ledger_cursor_read_u8(&cursor, &mcu_hash_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_mcu_version;
	}

	uint8_t *mcu_hash = calloc(mcu_hash_len, sizeof(uint8_t));
	if (!mcu_hash) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
		goto err_destroy_mcu_version;
	}

	if (!ledger_cursor_read_bytes(&cursor, mcu_hash, mcu_hash_len)) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_IO);
		goto err_destroy_mcu_hash;
	}

	struct ledger_bolos_version *_version = malloc(sizeof(struct ledger_bolos_version));
	if (!_version) {
		LEDGER_SET_ERROR(device, LEDGER_ERROR_NOMEM);
		goto err_destroy_mcu_hash;
	}

	_version->target_id = target_id;
	_version->os_version = os_version;
	_version->flags.len = flags_len;
	_version->flags.flags = flags;
	_version->mcu_version = mcu_version;
	_version->mcu_hash.len = mcu_hash_len;
	_version->mcu_hash.hash = mcu_hash;

	*version = _version;

	return true;

err_destroy_mcu_hash:
	free(mcu_hash);
err_destroy_mcu_version:
	free(mcu_version);
err_destroy_flags:
	free(flags);
err_destroy_os_version:
	free(os_version);
err_destroy_apdu_reply:
	ledger_apdu_reply_destroy(reply);
	return false;
}
