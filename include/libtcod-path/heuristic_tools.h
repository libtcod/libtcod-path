#pragma once

#include "heuristic_types.h"

/// @brief Return the result of `heuristic` at `index` for `distance`.
static inline int TCODPATH_heuristic_at(
    TCODPATH_Heuristic* __restrict heuristic, const int* __restrict index, int distance) {
  if (!heuristic || !index) return distance;
  return distance;
}
