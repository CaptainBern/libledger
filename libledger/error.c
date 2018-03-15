#include "libledger/error.h"

static const char *ledger_error_unknown = "unknown error";

struct error_description {
	int error_code;
	const char *error_str;
};

static const struct error_description error_description_table[] = {
	{ LEDGER_SUCCESS, "success" },

	{ LEDGER_ERROR_INTERNAL, "internal error" },
	{ LEDGER_ERROR_NOMEM, "no memory" },
	{ LEDGER_ERROR_NULL, "NULL pointer" },

	{ LEDGER_ERROR_IO, "I/O error" },

	{ LEDGER_ERROR_PROTOCOL, "protocol error" },
	{ LEDGER_ERROR_INVALID_LENGTH, "invalid length" },
	{ LEDGER_ERROR_UNEXPECTED_REPLY, "unexpected reply" },
	{ LEDGER_ERROR_INVALID_COMMAND, "invalid command" },

	{ LEDGER_ERROR_OTHER, "other error" }
};

const char *ledger_error_str(int error)
{
	for (int i = 0; i < (sizeof error_description_table / sizeof(struct error_description)); i++) {
		if (error_description_table[i].error_code == error) {
			return error_description_table[i].error_str;
		}
	}

	return ledger_error_unknown;
}
