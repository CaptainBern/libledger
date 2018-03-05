#include "libledger/apdu.h"
#include "libledger/error.h"
#include "libledger/transport.h"
#include "libledger/internal/binary.h"

#include "libledger/bolos/apdu.h"
#include "libledger/bolos/exchange_nonce.h"

int ledger_bolos_exchange_nonce(struct ledger_device *device, uint16_t comm_channel_id,
		uint8_t *nonce, size_t nonce_len, struct ledger_bolos_nonce_exchange *exchange) {
	int ret = 0;

	uint8_t apdu[LEDGER_APDU_HEADER_LENGTH + nonce_len];
	apdu[LEDGER_APDU_OFFSET_CLA] = LEDGER_BOLOS_APDU_CLA;
	apdu[LEDGER_APDU_OFFSET_INS] = LEDGER_BOLOS_APDU_INS_EXCHANGE_NONCE;
	apdu[LEDGER_APDU_OFFSET_P1] = 0x00;
	apdu[LEDGER_APDU_OFFSET_P2] = 0x00;
	apdu[LEDGER_APDU_OFFSET_LC] = nonce_len;
	binary_hton(&apdu[LEDGER_APDU_OFFSET_CDATA], &nonce[0], nonce_len);

	ret = ledger_transport_write_apdu(device, comm_channel_id, &apdu[0], sizeof apdu);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	struct ledger_transport_apdu_reply *reply = NULL;
	ret = ledger_transport_read_apdu(device, comm_channel_id, &reply);
	if (ret != LEDGER_SUCCESS) {
		return ret;
	}

	size_t offset = 0;

	uint8_t signer_serial[LEDGER_BOLOS_SIGNER_SERIAL_LENGTH];
	uint8_t device_nonce[LEDGER_BOLOS_NONCE_LENGTH];

	// Check if the reply is successful
	if (reply->data_len < 2) {
		ret = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	if (!ledger_apdu_sw_ok(reply->data[reply->data_len - 2], reply->data[reply->data_len - 1])) {
		ret = LEDGER_ERROR_UNKNOWN;
		goto error;
	}

	// Read the signer serial
	if (reply->data_len < sizeof signer_serial) {
		ret = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	offset += binary_ntoh(&reply->data[offset], &signer_serial[0], sizeof signer_serial);

	// Read the device nonce
	if ((reply->data_len - offset) < sizeof device_nonce) {
		ret = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	offset += binary_hton(&reply->data[offset], &device_nonce[0], sizeof device_nonce);

	// Check the status bytes
	if ((reply->data_len - offset) != 2) {
		ret = LEDGER_ERROR_INCORRECT_LENGTH;
		goto error;
	}

	// Set the exchange values
	if (exchange) {
		binary_hton(&exchange->signer_serial[0], &signer_serial[0], sizeof signer_serial);
		binary_hton(&exchange->device_nonce[0], &device_nonce[0], sizeof device_nonce);
	}
	ret = LEDGER_SUCCESS;

error:
	ledger_transport_free_apdu_reply(reply);
	return ret;
}
