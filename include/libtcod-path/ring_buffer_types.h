#pragma once

#include <stddef.h>

typedef struct TCODPATH_RingBuffer {
  ptrdiff_t capacity;  // Size of the buffer in bytes
  ptrdiff_t used;  // Number of bytes currently used
  ptrdiff_t begin;  // Byte index of the buffer head
  ptrdiff_t end;  // Byte index of the buffer tail
  unsigned char* __restrict data;
} TCODPATH_RingBuffer;
