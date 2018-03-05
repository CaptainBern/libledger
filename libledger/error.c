#include "libledger/error.h"

static const char *ledger_error_table[LEDGER_ERROR_COUNT] = {
	"Success",
	"Internal error",
	"Out of memory",
	"I/O error",
	"Incorrect length",
	"Invalid command",
	"Unexpected reply",
	"Unknown error"
};

const char *ledger_error_string(enum ledger_error error)
{
	int err_index = -error;

	if ((err_index < 0) || (err_index >= LEDGER_ERROR_COUNT)) {
		err_index = LEDGER_ERROR_COUNT - 1;
	}

	return ledger_error_table[err_index];
}
