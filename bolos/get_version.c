#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "libledger/apdu.h"
#include "libledger/error.h"
#include "libledger/transport.h"
#include "libledger/internal/binary.h"

#include "libledger/bolos/apdu.h"
#include "libledger/bolos/get_version.h"

static inline bool check_len(size_t len, size_t offset, size_t required)
{
	return (len - offset) > required;
}

struct ledger_bolos_version *read_version(const uint8_t *data, size_t data_len)
{
	uint32_t target_id = 0;

	uint8_t os_version_len = 0;
	char *os_version = NULL;

	uint8_t flags_len = 0;
	uint8_t *flags = NULL;

	uint8_t mcu_version_len = 0;
	char *mcu_version = NULL;

	size_t offset = 0;

	// Read the target ID
	if (!check_len(data_len, offset, sizeof target_id)) {
		return NULL;
	}

	offset += binary_ntoh_uint32(&data[offset], &target_id);

	// Read the OS version
	if (!check_len(data_len, offset, sizeof os_version_len)) {
		return NULL;
	}

	offset += binary_ntoh_uint8(&data[offset], &os_version_len);
	if (!check_len(data_len, offset, os_version_len)) {
		return NULL;
	}

	os_version = calloc(os_version_len, sizeof(char));
	if (!os_version) {
		return NULL;
	}

	offset += binary_ntoh(&data[offset], os_version, os_version_len);

	// Read the flags
	if (!check_len(data_len, offset, sizeof flags_len)) {
		goto err_free_os_version;
	}

	offset += binary_ntoh_uint8(&data[offset], &flags_len);
	if (!check_len(data_len, offset, flags_len)) {
		goto err_free_os_version;
	}

	flags = calloc(flags_len, sizeof(uint8_t));
	if (!flags) {
		goto err_free_os_version;
	}

	offset += binary_ntoh(&data[offset], flags, flags_len);

	// Read the MCU version
	if (!check_len(data_len, offset, sizeof mcu_version_len)) {
		goto err_free_flags;
	}

	offset += binary_ntoh_uint8(&data[offset], &mcu_version_len);
	if (!check_len(data_len, offset, mcu_version_len)) {
		goto err_free_flags;
	}

	mcu_version = calloc(mcu_version_len, sizeof(char));
	if (!mcu_version) {
		goto err_free_flags;
	}

	offset += binary_ntoh(&data[offset], mcu_version, mcu_version_len);

	// Check SW
	if ((data_len - offset) != 2) {
		goto err_free_mcu_version;
	}

	struct ledger_bolos_version *version = malloc(sizeof(struct ledger_bolos_version));
	if (!version) {
		goto err_free_mcu_version;
	}

	version->target_id = target_id;
	version->os_version = os_version;
	version->flags_len = flags_len;
	version->flags = flags;
	version->mcu_version = mcu_version;

	return version;

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
		free(version->flags);
		free(version->mcu_version);
		free(version);
	}
}
