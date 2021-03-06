#ifndef LIBLEDGER_ERROR_H_
#define LIBLEDHER_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Every error is split up in 2 parts. The first 8 bits
 * are the error type, the remaining 24 bits are the error
 * value bits. The error value bits allows applications to
 * check for the error type, and then decide whether they care
 * about the specifics.
 */
#define LEDGER_ERROR_VALUE_BITS 24

/**
 * enum ledger_error_type - An error type.
 */
enum ledger_error_type {
	LEDGER_ERROR_TYPE_UNKNOWN = -1,
	LEDGER_ERROR_TYPE_SUCCESS = 0,
	LEDGER_ERROR_TYPE_INTERNAL,
	LEDGER_ERROR_TYPE_IO,
	LEDGER_ERROR_TYPE_PROTOCOL,
	LEDGER_ERROR_TYPE_OTHER,
	LEDGER_ERROR_TYPE_COUNT
};

#define LIBLEDGER_ERROR_SUCCESS_START (LEDGER_ERROR_TYPE_SUCCESS << LEDGER_ERROR_VALUE_BITS)
#define LIBLEDGER_ERROR_INTERNAL_START (LEDGER_ERROR_TYPE_INTERNAL << LEDGER_ERROR_VALUE_BITS)
#define LIBLEDGER_ERROR_IO_START (LEDGER_ERROR_TYPE_IO << LEDGER_ERROR_VALUE_BITS)
#define LIBLEDGER_ERROR_PROTOCOL_START (LEDGER_ERROR_TYPE_PROTOCOL << LEDGER_ERROR_VALUE_BITS)
#define LIBLEDGER_ERROR_OTHER_START (LEDGER_ERROR_TYPE_OTHER << LEDGER_ERROR_VALUE_BITS)

/**
 * enum ledger_error - An error code.
 */
enum ledger_error {
	/* LEDGER_ERROR_TYPE_SUCCESS */
	LEDGER_SUCCESS = LIBLEDGER_ERROR_SUCCESS_START,

	/* LEDGER_ERROR_TYPE_INTERNAL */
	LEDGER_ERROR_INTERNAL = LIBLEDGER_ERROR_INTERNAL_START,
	LEDGER_ERROR_NOMEM,
	LEDGER_ERROR_NULL,

	/* LEDGER_ERROR_TYPE_IO */
	LEDGER_ERROR_IO = LIBLEDGER_ERROR_IO_START,

	/* LEDGER_ERROR_TYPE_PROTOCOL */
	LEDGER_ERROR_PROTOCOL = LIBLEDGER_ERROR_PROTOCOL_START,
	LEDGER_ERROR_INVALID_LENGTH,
	LEDGER_ERROR_UNEXPECTED_REPLY,
	LEDGER_ERROR_INVALID_COMMAND,

	/* LEDGER_ERROR_TYPE_OTHER */
	LEDGER_ERROR_OTHER = LIBLEDGER_ERROR_OTHER_START,
	LEDGER_ERROR_BUFFER_TOO_SMALL
};

/**
 * ledger_error_get_type() - Get the type for the given error code.
 * @error: Error code to retrieve the type from.
 *
 * Return: The ledger_error_type for the given error code.
 */
static inline int ledger_error_get_type(int error)
{
	int type = (error >> LEDGER_ERROR_VALUE_BITS) & 0xff;
	if (type < LEDGER_ERROR_TYPE_UNKNOWN) {
		type = LEDGER_ERROR_TYPE_UNKNOWN;
	} else if (type > LEDGER_ERROR_TYPE_COUNT) {
		type = LEDGER_ERROR_TYPE_COUNT - 1;
	}
	return type;
}

/**
 * ledger_error_str() - Get a string for the given error code.
 * @error: Error code to return a string for.
 *
 * Return: A human-readable string for the given error code.
 *         Returns 'unknown error' in case the error code is invalid
 *         or unknown.
 */
extern const char *ledger_error_str(int error);

#undef LIBLEDGER_ERROR_VALUE_BITS
#undef LIBLEDGER_ERROR_SUCCESS_START
#undef LIBLEDGER_ERROR_INTERNAL_START
#undef LIBLEDGER_ERROR_IO_START
#undef LIBLEDGER_ERROR_PROTOCOL_START
#undef LIBLEDGER_ERROR_OTHER_START

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_ERROR_H_ */
