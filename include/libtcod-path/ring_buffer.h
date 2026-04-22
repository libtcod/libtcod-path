#pragma once

#include <stdlib.h>

#include "error.h"
#include "ring_buffer_types.h"

static inline void TCODPATH_ring_buffer_uninit(TCODPATH_RingBuffer* __restrict buffer) {
  if (buffer->data) free(buffer->data);
  buffer = {0};
}

static inline int TCODPATH_ring_buffer_pop(
    TCODPATH_RingBuffer* __restrict buffer, ptrdiff_t n_bytes, const void* __restrict data_out) {
  unsigned char* data = (unsigned char*)data_out;
  while (n_bytes) {
    if (buffer->used == 0) return -1;  // Data Underflow
    *data++ = buffer->data[buffer->begin];
    if (++buffer->begin == buffer->capacity) buffer->begin = 0;
    --buffer->used;
    --n_bytes;
  }
  return 0;
}

static inline int TCODPATH_ring_buffer_grow(TCODPATH_RingBuffer* __restrict buffer, ptrdiff_t size_bytes) {
  if (size_bytes == 0) size_bytes = 256;
  if (buffer->capacity >= size_bytes) return 0;
  TCODPATH_RingBuffer new_buffer = {};
  new_buffer.capacity = size_bytes;
  new_buffer.used = new_buffer.end = buffer->used;
  new_buffer.data = (unsigned char*)malloc(size_bytes);
  if (!new_buffer.data) return TCODPATH_E_OUT_OF_MEMORY;
  TCODPATH_ring_buffer_pop(buffer, buffer->used, new_buffer.data);
  TCODPATH_ring_buffer_uninit(buffer);
  *buffer = new_buffer;
  return 0;
}

static inline int TCODPATH_ring_buffer_append(
    TCODPATH_RingBuffer* __restrict buffer, ptrdiff_t n_bytes, const void* __restrict data_in) {
  const unsigned char* data = (const unsigned char*)data_in;
  while (n_bytes) {
    if (buffer->used == buffer->capacity) {
      const int err = TCODPATH_ring_buffer_grow(buffer, buffer->capacity * 2);
      if (err) return err;
    }
    buffer->data[buffer->end] = *data++;
    if (++buffer->end == buffer->capacity) buffer->end = 0;
    ++buffer->used;
    --n_bytes;
  }
  return 0;
}
