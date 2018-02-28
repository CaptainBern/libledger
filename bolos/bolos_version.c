#include <stdlib.h>

#include "libledger/apdu.h"
#include "libledger/error.h"
#include "libledger/transport.h"
#include "libledger/internal/binary.h"

#include "libledger/bolos/apdu.h"
#include "libledger/bolos/bolos_version.h"

int ledger_bolos_get_version(struct ledger_device *device, struct ledger_bolos_version **version) {
	int ret = 0;
	int err = 0;

	ret = ledger_transport_ping(device);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	uint8_t apdu[LEDGER_APDU_HEADER_SIZE];
	apdu[LEDGER_APDU_OFFSET_CLA] = LEDGER_BOLOS_APDU_CLA;
	apdu[LEDGER_APDU_OFFSET_INS] = LEDGER_BOLOS_APDU_INS_GET_VERSION;
	apdu[LEDGER_APDU_OFFSET_P1] = 0x00;
	apdu[LEDGER_APDU_OFFSET_P2] = 0x00;
	apdu[LEDGER_APDU_OFFSET_LC] = 0x00;

	ret = ledger_transport_write_apdu(device, LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID, &apdu[0], sizeof apdu);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	struct ledger_transport_apdu_reply *reply;
	ret = ledger_transport_read_apdu(device, LEDGER_TRANSPORT_DEFAULT_COMM_CHANNEL_ID, &reply);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	/**
	 * 4 fields + 2 status words
	 * +----------------------------------+
	 * | name        |             length |
	 * +----------------------------------+
	 * | target_id   |                  4 |
	 * | os_version  | prefixed by 1 byte |
	 * | flags       | prefixed by 1 byte |
	 * | mcu_version | prefixed by 1 byte |
	 * +----------------------------------+
	 * | statuswords |                  2 |
	 * +----------------------------------+
	 */

	size_t offset = 0;

	uint32_t target_id = 0;

	uint8_t os_version_len = 0;
	char *os_version = NULL;

	uint8_t flags_len = 0;
	uint8_t *flags = NULL;

	uint8_t mcu_version_len = 0;
	char *mcu_version = NULL;

	// Check if the reply is successful early on
	if (reply->data_len < 2) {
		err = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	if (!ledger_apdu_sw_status_ok(reply->data[reply->data_len - 2], reply->data[reply->data_len - 1])) {
		err = LEDGER_ERROR_UNKNOWN; // TODO: better error code for this?
		goto error;
	}

	// Read the target ID
	if (reply->data_len < 4) {
		err = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	offset += binary_ntoh_uint32(&reply->data[offset], &target_id);

	// Read the OS version
	if ((reply->data_len - offset) < 1) {
		err = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	offset += binary_ntoh_uint8(&reply->data[offset], &os_version_len);
	if ((reply->data_len - offset) < os_version_len) {
		err = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	os_version = calloc(os_version_len, sizeof(char));
	if (!os_version) {
		err = LEDGER_ERROR_NOMEM;
		goto error;
	}

	offset += binary_ntoh(&reply->data[offset], os_version, os_version_len);

	// Read the flags
	if ((reply->data_len - offset) < 1) {
		err = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	offset += binary_ntoh_uint8(&reply->data[offset], &flags_len);
	if ((reply->data_len - offset) < flags_len) {
		err = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	flags = calloc(flags_len, sizeof(uint8_t));
	if (!flags) {
		err = LEDGER_ERROR_NOMEM;
		goto error;
	}

	offset += binary_ntoh(&reply->data[offset], flags, flags_len);

	// Read the MCU version
	if ((reply->data_len - offset) < 1) {
		err = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	offset += binary_ntoh_uint8(&reply->data[offset], &mcu_version_len);
	if ((reply->data_len - offset) < mcu_version_len) {
		err = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	mcu_version = calloc(mcu_version_len, sizeof(char));
	if (!mcu_version) {
		err = LEDGER_ERROR_NOMEM;
		goto error;
	}

	offset += binary_ntoh(&reply->data[offset], mcu_version, mcu_version_len);

	// Check the status bytes
	if ((reply->data_len - offset) != 2) {
		err = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	// Set the version
	struct ledger_bolos_version *_version = malloc(sizeof(struct ledger_bolos_version));
	if (!_version) {
		err = LEDGER_ERROR_NOMEM;
		goto error;
	}

	_version->target_id = target_id;
	_version->os_version = os_version;
	_version->flags_len = flags_len;
	_version->flags = flags;
	_version->mcu_version = mcu_version;
	*version = _version;

	ledger_transport_free_apdu_reply(reply);

	return LEDGER_SUCCESS;

error:
	ledger_transport_free_apdu_reply(reply);
	free(os_version);
	free(flags);
	free(mcu_version);
	return err;
}

void ledger_bolos_free_version(struct ledger_bolos_version *version) {
	if (version) {
		free(version->os_version);
		free(version->flags);
		free(version->mcu_version);
		free(version);
	}
}
