
#pragma once
#ifndef TCODPATH_HEAPQ_TYPES_H
#define TCODPATH_HEAPQ_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct TCODPATH_Heap {
  unsigned char* __restrict heap;
  int size;  // The current number of elements in heap.
  int capacity;  // The current capacity of heap.
  ptrdiff_t node_size;  // The full size of each node in bytes.
  ptrdiff_t data_size;  // The size of a nodes user data section in bytes.
  ptrdiff_t data_offset;  // The offset of the user data section.
  int priority_type;  // Bytesize and sign of priority type, should be -4 for int32
};
#endif  // TCODPATH_HEAPQ_TYPES_H
