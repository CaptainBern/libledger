#ifndef LIBLEDGER_INTERNAL_CURSOR_H_
#define LIBLEDGER_INTERNAL_CURSOR_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ledger_cursor {
	uintptr_t cursor;
	struct {
		uint8_t *data;
		size_t len;
	} buffer;
};

extern void ledger_cursor_init(struct ledger_cursor *cursor, uint8_t *data, size_t len);

extern void ledger_cursor_reset(struct ledger_cursor *cursor);

extern void ledger_cursor_wipe(struct ledger_cursor *cursor);

extern bool ledger_cursor_skip(struct ledger_cursor *cursor, size_t skip);

extern bool ledger_cursor_copy(struct ledger_cursor *to, struct ledger_cursor *from, size_t len);

extern bool ledger_cursor_read_bytes(struct ledger_cursor *cursor, uint8_t *out, size_t len);

extern bool ledger_cursor_read_u8(struct ledger_cursor *cursor, uint8_t *out);

extern bool ledger_cursor_read_u16(struct ledger_cursor *cursor, uint16_t *out);

extern bool ledger_cursor_read_u32(struct ledger_cursor *cursor, uint32_t *out);

extern bool ledger_cursor_read_u64(struct ledger_cursor *cursor, uint64_t *out);

extern bool ledger_cursor_write_bytes(struct ledger_cursor *cursor, const uint8_t *val, size_t len);

extern bool ledger_cursor_write_u8(struct ledger_cursor *cursor, const uint8_t val);

extern bool ledger_cursor_write_u16(struct ledger_cursor *cursor, const uint16_t val);

extern bool ledger_cursor_write_u32(struct ledger_cursor *cursor, const uint32_t val);

extern bool ledger_cursor_write_u64(struct ledger_cursor *cursor, const uint64_t val);

/*
 * Return how much data is left.
 */
static inline size_t ledger_cursor_remaining(struct ledger_cursor *cursor)
{
	return (cursor->buffer.len - cursor->cursor);
}

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_INTERNAL_CURSOR_H_ */
