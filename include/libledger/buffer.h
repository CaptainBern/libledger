#ifndef LIBLEDGER_BUFFER_H_
#define LIBLEDGER_BUFFER_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ledger_buffer {
	size_t len;
	uint8_t *data;
};

extern void ledger_buffer_init(struct ledger_buffer *buffer, size_t len, const uint8_t *data);

extern struct ledger_buffer *ledger_buffer_create(size_t len);

extern void ledger_buffer_destroy(struct ledger_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_BUFFER_H_ */
