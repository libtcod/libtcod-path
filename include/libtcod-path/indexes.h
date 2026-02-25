#pragma once
#include "config.h"
/// @brief Begin iterating over the indexes of `shape_ij` in row-major order.
/// Used as the init element for a for loop
/// @param n Length of the provided `shape` and `index_out` arrays
/// @param index_out Output to hold the current index, can be NULL
/// @details Invalid or NULL values are safe to use.
/// @code{.c}
/// {
///   int shape[2] = {10, 20};
///   int index[2];
///   for (TCODPATH_indexes_iter_begin(2, index); TCODPATH_indexes_iter_step(2, shape, index);) {
///     // Operate on each index
///   }
/// }
/// @endcode{.c}
static inline void TCODPATH_indexes_iter_begin(int n, int* __restrict index_out) {
  if (!index_out) return;  // Missing output array
  if (n < 0 || n > TCODPATH_MAX_DIMENSIONS) return;  // Invalid length
  for (int i = 0; i < n; ++i) index_out[i] = 0;
  index_out[n - 1] = -1;  // Next step will increment this to zero
}
/// @brief Step over the indexes of `shape`. Return false after the end has been reached.
/// Used as the step element of a for loop.
/// @param n Length of the provided `shape` and `index_out` arrays
/// @param shape The shape to iterate over, can be NULL
/// @param index_out Output to hold the current index, can be NULL
/// @return `true` if the current `index_out` is valid, `false` otherwise.
static inline bool TCODPATH_indexes_iter_step(int n, const int* __restrict shape, int* __restrict index_out) {
  if (!index_out || !shape) return false;  // Missing array pointer
  if (n < 0 || n > TCODPATH_MAX_DIMENSIONS) return false;  // Invalid length
  for (int i = n - 1; i >= 0; --i) {  // Increment index in row-major order
    ++index_out[i];
    if (index_out[i] < shape[i]) return true;  // return next valid index
    index_out[i] = 0;
  }
  return false;  // end has been reached
}
