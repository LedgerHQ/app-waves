#ifndef BUFFER_HELPER_H
#define BUFFER_HELPER_H

#include "buffer.h"
#include <string.h>
#include <stdbool.h>



static inline bool buffer_read_next(buffer_t *buffer, uint8_t *out, size_t len) {
    if (buffer->offset + len > buffer->size) {
        return false;
    }
    memmove(out, buffer->ptr + buffer->offset, len);
    buffer->offset += len;
    return true;
}

static inline bool buffer_read_range(const buffer_t *buffer, uint8_t *out, size_t from, size_t to) {
    if (from > to || to >= buffer->size) {
        return false;
    }
    size_t len = to - from + 1;
    memmove(out, buffer->ptr + from, len);
    return true;
}

static inline bool buffer_seek_next(buffer_t *buffer, size_t len) {
    if (buffer->offset + len > buffer->size) {
        return false;
    }
    buffer->offset += len;
    return true;
}

static inline size_t buffer_read_remaining(buffer_t *buffer, uint8_t *out) {
    size_t remaining = buffer->size - buffer->offset;
    if (remaining > 0) {
        memmove(out, buffer->ptr + buffer->offset, remaining);
        buffer->offset = buffer->size;
        return remaining;
    }
    return 0;
}

static inline bool read_path_from_buffer(buffer_t *cdata, uint32_t *path, size_t path_len) {
    for (size_t i = 0; i < path_len; i++) {
        // Убедитесь, что buffer_read_u32 доступен (он в buffer.h)
        if (!buffer_read_u32(cdata, &path[i], BE)) {
            return false;
        }
    }
    return true;
}

static inline size_t buffer_remaining(const buffer_t *buffer) {
    if (buffer->offset >= buffer->size) {
        return 0;
    }
    return buffer->size - buffer->offset;
}
#endif // BUFFER_HELPER_H