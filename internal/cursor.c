#include <string.h>

#include "internal/cursor.h"

void ledger_cursor_init(struct ledger_cursor *cursor, uint8_t *data, size_t len)
{
	cursor->buffer.data = data;
	cursor->buffer.len = len;
	cursor->read_cursor = 0;
	cursor->write_cursor = 0;
}

void ledger_cursor_reset_read(struct ledger_cursor *cursor)
{
	cursor->read_cursor = 0;
}

bool ledger_cursor_skip_read(struct ledger_cursor *cursor, size_t len)
{
	if (ledger_cursor_remaining(cursor) < len) {
		return false;
	}

	cursor->read_cursor += len;
	return true;
}

bool ledger_cursor_read_bytes(struct ledger_cursor *cursor, uint8_t *out, size_t len)
{
	if (ledger_cursor_remaining(cursor) < len) {
		return false;
	}

	void *ptr = cursor->buffer.data + cursor->read_cursor;
	if (!ptr) {
		return false;
	}

	memcpy(out, ptr, len);
	cursor->read_cursor += len;
	return true;
}

bool ledger_cursor_read(struct ledger_cursor *cursor, struct ledger_buffer *buffer)
{
	return ledger_cursor_read_bytes(cursor, buffer->data, buffer->len);
}

bool ledger_cursor_read_u8(struct ledger_cursor *cursor, uint8_t *out)
{
	return ledger_cursor_read_bytes(cursor, out, 1);
}

bool ledger_cursor_read_u16(struct ledger_cursor *cursor, uint16_t *out)
{
	uint8_t buffer[2];

	if (!ledger_cursor_read_bytes(cursor, buffer, sizeof buffer)) {
		return false;
	}

	if (out) {
		*out = (uint16_t) buffer[1];
		*out |= (uint16_t) buffer[0] << 8;
	}
	return true;
}

bool ledger_cursor_read_u32(struct ledger_cursor *cursor, uint32_t *out)
{
	uint8_t buffer[4];

	if (!ledger_cursor_read_bytes(cursor, buffer, sizeof buffer)) {
		return false;
	}

	if (out) {
		*out = (uint32_t) buffer[3];
		*out |= (uint32_t) buffer[2] << 8;
		*out |= (uint32_t) buffer[1] << 16;
		*out |= (uint32_t) buffer[0] << 24;
	}
	return true;
}

bool ledger_cursor_read_u64(struct ledger_cursor *cursor, uint64_t *out)
{
	uint8_t buffer[8];

	if (!ledger_cursor_read_bytes(cursor, buffer, sizeof buffer)) {
		return false;
	}

	if (out) {
		*out = (uint64_t) buffer[7];
		*out |= (uint64_t) buffer[6] << 8;
		*out |= (uint64_t) buffer[5] << 16;
		*out |= (uint64_t) buffer[4] << 24;
		*out |= (uint64_t) buffer[3] << 32;
		*out |= (uint64_t) buffer[2] << 40;
		*out |= (uint64_t) buffer[1] << 48;
		*out |= (uint64_t) buffer[0] << 56;
	}
	return true;
}

void ledger_cursor_reset_write(struct ledger_cursor *cursor)
{
	cursor->write_cursor = 0;
}

bool ledger_cursor_skip_write(struct ledger_cursor *cursor, size_t len)
{
	if (ledger_cursor_available(cursor) < len) {
		return false;
	}

	cursor->write_cursor += len;
	return true;
}

bool ledger_cursor_write_bytes(struct ledger_cursor *cursor, const uint8_t *val, size_t len)
{
	if (ledger_cursor_available(cursor) < len) {
		return false;
	}

	void *ptr = cursor->buffer.data + cursor->write_cursor;
	if (!ptr) {
		return false;
	}

	memcpy(ptr, val, len);
	cursor->write_cursor += len;
	return true;
}

bool ledger_cursor_write(struct ledger_cursor *cursor, const struct ledger_buffer *buffer)
{
	return ledger_cursor_write_bytes(cursor, buffer->data, buffer->len);
}

bool ledger_cursor_write_u8(struct ledger_cursor *cursor, const uint8_t val)
{
	return ledger_cursor_write_bytes(cursor, &val, 1);
}

bool ledger_cursor_write_u16(struct ledger_cursor *cursor, const uint16_t val)
{
	uint8_t buffer[2];
	buffer[0] = (val >> 8) & 0xff;
	buffer[1] = val & 0xff;

	return ledger_cursor_write_bytes(cursor, buffer, sizeof buffer);
}

bool ledger_cursor_write_u32(struct ledger_cursor *cursor, const uint32_t val)
{
	uint8_t buffer[4];
	buffer[0] = (val >> 24) & 0xff;
	buffer[1] = (val >> 16) & 0xff;
	buffer[2] = (val >> 8) & 0xff;
	buffer[3] = val & 0xff;

	return ledger_cursor_write_bytes(cursor, buffer, sizeof buffer);
}

bool ledger_cursor_write_u64(struct ledger_cursor *cursor, const uint64_t val)
{
	uint8_t buffer[8];
	buffer[0] = (val >> 56) & 0xff;
	buffer[1] = (val >> 48) & 0xff;
	buffer[2] = (val >> 40) & 0xff;
	buffer[3] = (val >> 32) & 0xff;
	buffer[4] = (val >> 24) & 0xff;
	buffer[5] = (val >> 16) & 0xff;
	buffer[6] = (val >> 8) & 0xff;
	buffer[7] = val & 0xff;

	return ledger_cursor_write_bytes(cursor, buffer, sizeof buffer);
}

bool ledger_cursor_copy(struct ledger_cursor *to, struct ledger_cursor *from, size_t len)
{
	if (ledger_cursor_remaining(from) < len) {
		return false;
	}

	if (ledger_cursor_available(to) < len) {
		return false;
	}

	void *ptr_to = to->buffer.data + to->write_cursor;
	if (!ptr_to) {
		return false;
	}

	void *ptr_from = from->buffer.data + from->read_cursor;
	if (!ptr_from) {
		return false;
	}

	memcpy(ptr_to, ptr_from, len);
	to->write_cursor += len;
	from->read_cursor += len;
	return true;
}

void ledger_cursor_reset(struct ledger_cursor *cursor)
{
	ledger_cursor_reset_read(cursor);
	ledger_cursor_reset_write(cursor);
}

void ledger_cursor_wipe(struct ledger_cursor *cursor)
{
	memset(cursor->buffer.data, 0, cursor->buffer.len);
	ledger_cursor_reset(cursor);
}
