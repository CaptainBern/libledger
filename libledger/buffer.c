#include <stdlib.h>

#include "libledger/buffer.h"

void ledger_buffer_init(struct ledger_buffer *buffer, uint8_t *data, size_t len)
{
	buffer->data = data;
	buffer->len = len;
}

struct ledger_buffer *ledger_buffer_create(size_t len)
{
	struct ledger_buffer *buffer = malloc(sizeof(struct ledger_buffer));
	if (!buffer) {
		return NULL;
	}

	uint8_t *data = calloc(len, sizeof(uint8_t));
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
