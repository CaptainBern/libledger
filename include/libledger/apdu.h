#ifndef LIBLEDGER_APDU_H_
#define LIBLEDGER_APDU_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEDGER_APDU_HEADER_LENGTH   0x05
#define LEDGER_APDU_MAX_DATA_LENGTH 0xff

#define LEDGER_APDU_MAX_LENGTH      (LEDGER_APDU_HEADER_SIZE + LEDGER_APDU_MAX_DATA_LENGTH)

#define LEDGER_APDU_OFFSET_CLA      0x00
#define LEDGER_APDU_OFFSET_INS      0x01
#define LEDGER_APDU_OFFSET_P1       0x02
#define LEDGER_APDU_OFFSET_P2       0x03
#define LEDGER_APDU_OFFSET_LC       0x04
#define LEDGER_APDU_OFFSET_CDATA    0x05

#define LEDGER_APDU_OK(x, y) (((x) == 0x90) && ((y) == 0x00))

static inline bool ledger_apdu_ok(uint8_t *apdu, size_t apdu_len)
{
	if (apdu_len < 2) {
		return false;
	}
	return LEDGER_APDU_OK(apdu[apdu_len - 2], apdu[apdu_len - 1]);
}

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_APDU_H_ */
