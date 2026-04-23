#pragma once

#include <stdlib.h>

#include "error.h"
#include "ring_buffer_types.h"

/// @brief Free arrays from a ring buffer and reset it to the default zero state.
static inline void TCODPATH_ring_buffer_uninit(TCODPATH_RingBuffer* __restrict buffer) {
  if (!buffer) return;
  if (buffer->data) free(buffer->data);
  buffer = {0};
}
/// @brief Pop data from the left of a ring buffer.
/// @param buffer Must not be `NULL`.
/// @param n_bytes Length of `data_out` array in bytes.
/// @param data_out Output to store popped data. Can be `NULL`.
/// @return 0 on success, negative value on buffer underflow.
static inline int TCODPATH_ring_buffer_pop(
    TCODPATH_RingBuffer* __restrict buffer, ptrdiff_t n_bytes, const void* __restrict data_out) {
  unsigned char* data = (unsigned char*)data_out;
  while (n_bytes) {
    if (buffer->used_bytes == 0) return -1;  // Data Underflow
    if (data) *data++ = buffer->data[buffer->begin];
    if (++buffer->begin == buffer->capacity) buffer->begin = 0;
    --buffer->used_bytes;
    --n_bytes;
  }
  return 0;
}
/// @brief Increate buffer capacity to at least `size_bytes`.
/// @param buffer Must not be `NULL`.
/// @param size_bytes Can be zero for a decent default.
/// @return 0 on success, negative on error.
static inline int TCODPATH_ring_buffer_grow(TCODPATH_RingBuffer* __restrict buffer, ptrdiff_t size_bytes) {
  if (size_bytes == 0) size_bytes = 256;
  if (buffer->capacity >= size_bytes) return 0;
  TCODPATH_RingBuffer new_buffer = {};
  new_buffer.capacity = size_bytes;
  new_buffer.used_bytes = new_buffer.end = buffer->used_bytes;
  new_buffer.data = (unsigned char*)malloc(size_bytes);
  if (!new_buffer.data) return TCODPATH_E_OUT_OF_MEMORY;
  TCODPATH_ring_buffer_pop(buffer, buffer->used_bytes, new_buffer.data);
  TCODPATH_ring_buffer_uninit(buffer);
  *buffer = new_buffer;
  return 0;
}
/// @brief Append data to the right of a ring buffer.
/// @param buffer Must not be `NULL`.
/// @param n_bytes Size of `data_in` in bytes.
/// @param data_in Input value.
/// @return 0 on success, negative on error.
static inline int TCODPATH_ring_buffer_append(
    TCODPATH_RingBuffer* __restrict buffer, ptrdiff_t n_bytes, const void* __restrict data_in) {
  const unsigned char* data = (const unsigned char*)data_in;
  while (n_bytes) {
    if (buffer->used_bytes == buffer->capacity) {
      const int err = TCODPATH_ring_buffer_grow(buffer, buffer->capacity * 2);
      if (err) return err;
    }
    buffer->data[buffer->end] = *data++;
    if (++buffer->end == buffer->capacity) buffer->end = 0;
    ++buffer->used_bytes;
    --n_bytes;
  }
  return 0;
}
