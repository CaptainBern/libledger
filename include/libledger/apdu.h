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

enum ledger_apdu_sw {
	LEDGER_APDU_SW_NO_INFO_GIVEN                   = 0x6200,
	LEDGER_APDU_SW_DATA_MAY_BE_CORRUPTED           = 0x6281,
	LEDGER_APDU_SW_EOF                             = 0x6282,
	LEDGER_APDU_SW_INVALID_DF                      = 0x6283,
	LEDGER_APDU_SW_SELECTED_FILE_INVALID           = 0x6284,
	LEDGER_APDU_SW_AUTH_FAILED                     = 0x6300,
	LEDGER_APDU_SW_FILE_FULL                       = 0x6381,
	LEDGER_APDU_SW_MEMORY_FAILURE                  = 0x6501,
	LEDGER_APDU_SW_MEMORY_FAILURE_2                = 0x6580,
	LEDGER_APDU_SW_INCORRECT_LENGTH                = 0x6700,
	LEDGER_APDU_SW_FUNCTION_NOT_SUPPORTED          = 0x6800,
	LEDGER_APDU_SW_LOGICAL_CHANNEL_NOT_SUPPORTED   = 0x6881,
	LEDGER_APDU_SW_SECURE_MESSAGING_NOT_SUPPORTED  = 0x6882,
	LEDGER_APDU_SW_NO_SUCCESSFUL_TRANSACTION       = 0x6900,
	LEDGER_APDU_SW_CANNOT_SELECT_FILE              = 0x6981,
	LEDGER_APDU_SW_ACCESS_CONDITIONS_NOT_FULFILLED = 0x6982,
	LEDGER_APDU_SW_SECRET_CODE_LOCKED              = 0x6983,
	LEDGER_APDU_SW_REFERENCED_DATA_INVALID         = 0x6984,
	LEDGER_APDU_SW_NO_SELECTED_EF                  = 0x6985,
	LEDGER_APDU_SW_COMMAND_NOT_ALLOWED             = 0x6986,
	LEDGER_APDU_SW_SM_DATA_OBJECTS_MISSING         = 0x6987,
	LEDGER_APDU_SW_SM_DATA_OBJECTS_INCORRECT       = 0x6988,
	LEDGER_APDU_SW_P1_P2_INCORRECT                 = 0x6A00,
	LEDGER_APDU_SW_DATA_PARAMETERS_INCORRECT       = 0x6A80,
	LEDGER_APDU_SW_COMMAND_NOT_SUPPORTED           = 0x6A81,
	LEDGER_APDU_SW_FILE_NOT_FOUND                  = 0x6A82,
	LEDGER_APDU_SW_RECORD_NOT_FOUND                = 0x6A83,
	LEDGER_APDU_SW_INSUFFICIENT_MEMORY_SPACE       = 0x6A84,
	LEDGER_APDU_SW_LC_INCONSISTENT_WITH_TLV        = 0x6A85,
	LEDGER_APDU_SW_INCORRECT_PARAMETERS_P1_P2      = 0x6A86,
	LEDGER_APDU_SW_P3_INCONSISTENT_WITH_P1_P2      = 0x6A87,
	LEDGER_APDU_SW_REFERENCED_DATA_NOT_FOUND       = 0x6A88,
	LEDGER_APDU_SW_INVALID_P1_P2                   = 0x6B00,
	LEDGER_APDU_SW_INCORRECT_P3_LENGTH             = 0x6CFF, // Special case, correct length is stored in 0x00FF
	LEDGER_APDU_SW_INVALID_INSTRUCTION             = 0x6D00,
	LEDGER_APDU_SW_INCORRECT_APPLICATION           = 0x6E00,
	LEDGER_APDU_SW_CHECKING_ERROR                  = 0x6F00,
	LEDGER_APDU_SW_SUCCESS                         = 0x9000
};

#define LEDGER_APDU_OK(x, y) (((x) == 0x90) && ((y) == 0x00))

static inline bool ledger_apdu_ok(uint8_t *apdu, size_t apdu_len)
{
	if (apdu_len < 2) {
		return false;
	}
	return LEDGER_APDU_OK(apdu[apdu_len - 2], apdu[apdu_len - 1]);
}

struct ledger_apdu_header {
	uint8_t cla;
	uint8_t ins;
	uint8_t p1;
	uint8_t p2;
	uint8_t lc;
};

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_APDU_H_ */
