#ifndef LIBLEDGER_APDU_H_
#define LIBLEDGER_APDU_H_

#include <stddef.h>
#include <stdint.h>

#include "libledger/core/device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDGER_APDU_HEADER_SIZE     0x05
#define LEDGER_APDU_MAX_DATA_LENGTH 0xff
#define LEDGER_APDU_MAX_LENGTH      (LEDGER_APDU_HEADER_SIZE + LEDGER_APDU_MAX_DATA_LENGTH)

#define LEDGER_APDU_OFFSET_CLA 0x00
#define LEDGER_APDU_OFFSET_INS 0x01
#define LEDGER_APDU_OFFSET_P1  0x02
#define LEDGER_APDU_OFFSET_P2  0x03

/**
 * 4 possible APDU cases:
 * Case 1: CLA | INS | P1 | P2
 * Case 2: CLA | INS | P1 | P2 | Le
 * Case 3: CLA | INS | P1 | P2 | Lc | Data
 * Case 4: CLA | INS | P1 | P2 | Lc | Data | Le
 *
 * Unlike fully ISO 7816 compliant devices, Ledger devices
 * only support short APDUs and thus the data payload can
 * only be 255 bytes at most.
 */
enum ledger_apdu_case {
    LEDGER_APDU_CASE_1,
    LEDGER_APDU_CASE_2,
    LEDGER_APDU_CASE_3,
    LEDGER_APDU_CASE_4
};

extern size_t ledger_apdu(void *out, size_t out_len, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2);

extern size_t ledger_apdu_le(void *out, size_t out_len, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t le);

extern size_t ledger_apdu_data(void *out, size_t out_len, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t *data, size_t data_len);

extern size_t ledger_apdu_data_le(void *out, size_t out_len, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t *data, size_t data_len, uint8_t le);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_APDU_H_ */
