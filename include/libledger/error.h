#ifndef LIBLEDGER_ERROR_H_
#define LIBLEDHER_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

enum ledger_error {
	LEDGER_SUCCESS = 0,
	LEDGER_ERROR_INTERNAL = -1,
	LEDGER_ERROR_NOMEM = -2,
	LEDGER_ERROR_IO = -4,
	LEDGER_ERROR_INCORRECT_LENGTH = -5,
	LEDGER_ERROR_INVALID_COMMAND = -6,
	LEDGER_ERROR_UNEXPECTED_REPLY = -7,
	LEDGER_ERROR_UNKNOWN = -99
};

#define LEDGER_ERROR_COUNT 8

extern const char *ledger_error_string(int error);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_ERROR_H_ */
