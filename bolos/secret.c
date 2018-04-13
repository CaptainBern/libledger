#include "internal/cursor.h"
#include "internal/macros.h"

#include "libledger/buffer.h"
#include "libledger/error.h"
#include "libledger/apdu.h"
#include "libledger/bolos/constants.h"

#include "libledger/bolos/secret.h"

bool ledger_bolos_exchange_nonce(struct ledger_device *device, uint16_t channel_id, uint8_t nonce[LEDGER_BOLOS_NONCE_LENGTH],
		uint8_t signer_serial[LEDGER_BOLOS_SIGNER_SERIAL_LENGTH], uint8_t device_nonce[LEDGER_BOLOS_NONCE_LENGTH])
{
	struct ledger_buffer buffer;
	ledger_buffer_init(&buffer, nonce, sizeof nonce);

	struct ledger_apdu_command command = LEDGER_APDU_COMMAND_INITIALIZER(
			LEDGER_BOLOS_APDU_CLA, LEDGER_BOLOS_APDU_INS_NONCE, 0x0, 0x0, &buffer);

	struct ledger_apdu_reply *reply = NULL;
	if (!ledger_apdu_exchange(device, channel_id, &command, &reply))
		return false;

	if (reply->status != 0x9000)
		goto err_destroy_apdu_reply;

	if (!reply->data)
		goto err_destroy_apdu_reply;

	struct ledger_cursor cursor;
	ledger_cursor_init(&cursor, reply->data->data, reply->data->len);

	if (!ledger_cursor_read_bytes(&cursor, signer_serial, sizeof(signer_serial)))
		goto err_destroy_apdu_reply;

	if (!ledger_cursor_read_bytes(&cursor, device_nonce, sizeof(device_nonce)))
		goto err_destroy_apdu_reply;

	return true;

err_destroy_apdu_reply:
	ledger_apdu_reply_destroy(reply);
	return false;
}

bool ledger_bolos_get_secret(struct ledger_device *device, uint16_t channel_id)
{

}
