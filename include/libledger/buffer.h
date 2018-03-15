#ifndef LIBLEDGER_BUFFER_H_
#define LIBLEDGER_BUFFER_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * struct ledger_buffer - A buffer.
 * @data: Pointer to the data in this buffer.
 * @len: Length of the data in this buffer.
 */
struct ledger_buffer {
	uint8_t *data;
	size_t len;
};

/**
 * ledger_buffer_init() - Initialize a buffer.
 * @buffer: Pointer to the buffer to initialize.
 * @data: The data to store in the buffer.
 * @len: The length of the data.
 *
 * This is a helper function to initialize buffers on the stack.
 */
extern void ledger_buffer_init(struct ledger_buffer *buffer, uint8_t *data, size_t len);

/**
 * ledger_buffer_create() - Create a new buffer.
 * @len: The amount of bytes the buffer can hold.
 *
 * Creates a new buffer on the heap.
 *
 * Return: A pointer to a ledger_buffer on success or NULL on failure.
 */
extern struct ledger_buffer *ledger_buffer_create(size_t len);

/**
 * ledger_buffer_destroy() - Destroy a buffer.
 * @buffer: A ledger_buffer pointer returned from
 *          ledger_buffer_create().
 *
 * Destroy a heap allocated buffer.
 */
extern void ledger_buffer_destroy(struct ledger_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_BUFFER_H_ */
