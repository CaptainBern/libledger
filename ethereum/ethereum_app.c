#include "libledger/core/apdu.h"
#include "libledger/core/error.h"
#include "libledger/core/transport.h"

#include "libledger/ethereum/apdu.h"
#include "libledger/ethereum/ethereum_app.h"

int ledger_ethereum_app_get_config(struct ledger_device *device, struct ledger_ethereum_app_config *config) {
	int ret = 0;

	ret = ledger_transport_ping(device);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	uint8_t apdu[LEDGER_APDU_HEADER_SIZE];
	apdu[LEDGER_APDU_OFFSET_CLA] = LEDGER_ETH_APDU_CLA;
	apdu[LEDGER_APDU_OFFSET_INS] = LEDGER_ETH_APDU_INS_GET_APP_CONFIGURATION;
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

	if (reply->data_len != 6) {
		ret = LEDGER_ERROR_INCORRECT_LENGTH;
		goto cleanup;
	}

	if (config) {
		config->flags = reply->data[0];
		config->version_major = reply->data[1];
		config->version_minor = reply->data[2];
		config->version_patch = reply->data[3];
	}

cleanup:
	ledger_transport_free_apdu_reply(reply);
	return ret;
}
