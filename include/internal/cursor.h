#ifndef LIBLEDGER_CURSOR_H_
#define LIBLEDGER_CURSOR_H_

#include <stddef.h>
#include <stdint.h>

#include "libledger/buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ledger_cursor {
	struct ledger_buffer buffer;
	uintptr_t read_cursor;
	uintptr_t write_cursor;
};

extern int ledger_cursor_init(struct ledger_cursor *cursor, struct ledger_buffer *buffer);

extern int ledger_cursor_reset(struct ledger_cursor *cursor);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_CURSOR_H_ */
