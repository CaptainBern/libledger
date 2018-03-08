#include <stdlib.h>

#include "libledger/buffer.h"

void ledger_buffer_init(struct ledger_buffer *buffer, size_t len, const uint8_t *data)
{
	buffer->len = len;
	buffer->data = data;
}

struct ledger_buffer *ledger_buffer_create(size_t len)
{
	struct ledger_buffer *buffer = malloc(sizeof(struct ledger_buffer));
	if (!buffer) {
		return NULL;
	}

	uint8_t *data = malloc(len);
	if (!data) {
		return NULL;
	}

	buffer->len = len;
	buffer->data = data;

	return buffer;
}

void ledger_buffer_destroy(struct ledger_buffer *buffer)
{
	if (buffer) {
		free(buffer->data);
		free(buffer);
	}
}
