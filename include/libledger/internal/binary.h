#ifndef LIBLEDGER_INTERNAL_BINARY_H_
#define LIBLEDGER_INTERNAL_BINARY_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern size_t binary_hton(void *out, const void *value, size_t value_len);

extern size_t binary_hton_uint8(void *out, uint8_t value);

extern size_t binary_hton_uint16(void *out, uint16_t value);

extern size_t binary_hton_uint32(void *out, uint32_t value);

extern size_t binary_hton_uint64(void *out, uint64_t value);

extern size_t binary_ntoh(const void *in, void *out, size_t out_len);

extern size_t binary_ntoh_uint8(const void *in, uint8_t *out);

extern size_t binary_ntoh_uint16(const void *in, uint16_t *out);

extern size_t binary_ntoh_uint32(const void *in, uint32_t *out);

extern size_t binary_ntoh_uint64(const void *in, uint64_t *out);

#ifdef __cplusplus
}
#endif

#endif /* LIBLEDGER_INTERNAL_BINARY_H_ */
