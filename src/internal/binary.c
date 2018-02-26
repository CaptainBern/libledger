#include <stdlib.h>
#include <string.h>

#include "libledger/internal/binary.h"

size_t binary_hton(void *out, const void *value, size_t value_len) {
    if (out) {
        memset(out, 0, value_len);
        memcpy(out, value, value_len);
        return value_len;
    } else {
        return 0;
    }
}

size_t binary_hton_uint8(void *out, uint8_t value) {
    return binary_hton(out, &value, sizeof value);
}

size_t binary_hton_uint16(void *out, uint16_t value) {
    uint8_t buffer[2];
    buffer[0] = (value >> 8) & 0xff;
    buffer[1] = value & 0xff;
    
    return binary_hton(
        out,
        &buffer,
        sizeof buffer
    );
}

size_t binary_hton_uint32(void *out, uint32_t value) {    
    uint8_t buffer[4];
    buffer[0] = (value >> 24) & 0xff;
    buffer[1] = (value >> 16) & 0xff;
    buffer[2] = (value >> 8) & 0xff;
    buffer[3] = value & 0xff;

    return binary_hton(
        out,
        &buffer,
        sizeof buffer 
    );
}

size_t binary_hton_uint64(void *out, uint64_t value) {
    uint8_t buffer[8];
    buffer[0] = (value >> 56) & 0xff;
    buffer[1] = (value >> 48) & 0xff;
    buffer[2] = (value >> 40) & 0xff;
    buffer[3] = (value >> 32) & 0xff;
    buffer[4] = (value >> 24) & 0xff;
    buffer[5] = (value >> 16) & 0xff;
    buffer[6] = (value >> 8) & 0xff;
    buffer[7] = value & 0xff;

    return binary_hton( 
        out,
        &buffer,
        sizeof buffer
    );
}

size_t binary_ntoh(const void *in, void *out, size_t out_len) {
    if (out) {
        memcpy(out, in, out_len);
        return out_len;
    } else {
        return 0;
    }
}

size_t binary_ntoh_uint8(const void *in, uint8_t *out) {
    uint8_t buffer[1];
    size_t r = binary_ntoh(in, &buffer, sizeof buffer);
    if (out) {
        *out = (uint8_t) buffer[0];
    }
    return r;
}

size_t binary_ntoh_uint16(const void *in, uint16_t *out) {
    uint8_t buffer[2];
    size_t r = binary_ntoh(in, &buffer, sizeof buffer);
    if (out) {
        *out = (uint16_t) buffer[1];
        *out |= (uint16_t) buffer[0] << 8;
    }
    return r;
}

size_t binary_ntoh_uint32(const void *in, uint32_t *out) {
    uint8_t buffer[4];
    size_t r = binary_ntoh(in, &buffer, sizeof buffer);
    if (out) {
        *out = (uint32_t) buffer[3];
        *out |= (uint32_t) buffer[2] << 8;
        *out |= (uint32_t) buffer[1] << 16;
        *out |= (uint32_t) buffer[0] << 24;
    }
    return r;
}

size_t binary_ntoh_uint64(const void *in, uint64_t *out) {
    uint8_t buffer[8];
    size_t r = binary_ntoh(in, &buffer, sizeof buffer);
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
    return r;
}