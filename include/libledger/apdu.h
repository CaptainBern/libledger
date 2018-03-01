#ifndef LIBLEDGER_APDU_H_
#define LIBLEDGER_APDU_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEDGER_APDU_HEADER_SIZE     0x05
#define LEDGER_APDU_MAX_DATA_LENGTH 0xff
#define LEDGER_APDU_MAX_LENGTH      (LEDGER_APDU_HEADER_SIZE + LEDGER_APDU_MAX_DATA_LENGTH)

#define LEDGER_APDU_OFFSET_CLA   0x00
#define LEDGER_APDU_OFFSET_INS   0x01
#define LEDGER_APDU_OFFSET_P1    0x02
#define LEDGER_APDU_OFFSET_P2    0x03
#define LEDGER_APDU_OFFSET_LC    0x04
#define LEDGER_APDU_OFFSET_CDATA 0x05

static inline bool ledger_apdu_sw_ok(uint8_t sw1, uint8_t sw2) {
	return (sw1 == 0x90) && (sw2 == 0x00);
}

extern const char *ledger_apdu_sw_description(uint8_t sw1, uint8_t sw2);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_APDU_H_ */
