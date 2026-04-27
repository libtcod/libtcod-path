#pragma once

#include "heuristic_types.h"
#include "utility.h"

/// @brief Return the result of `heuristic` at `index` for `distance`.
static inline int TCODPATH_heuristic_at(
    TCODPATH_Heuristic* __restrict heuristic,
    int dimensions,
    const TCODPATH_IndexType* __restrict index,
    TCODPATH_ValueType distance) {
  if (!heuristic || !index) return distance;
  switch (heuristic->type) {
    case TCODPATH_HEURISTIC_CALLBACK:
      return heuristic->callback.callback(heuristic->callback.userdata, dimensions, index, distance);
    case TCODPATH_HEURISTIC_BASIC:
      for (int i = 0; i < dimensions; ++i) {
        distance += TCODPATH_ABS(heuristic->basic.target[dimensions - i] - index[TCODPATH_MAX_DIMENSIONS - i]) *
                    heuristic->basic.greed[TCODPATH_MAX_DIMENSIONS - i];
      }
      return distance;
    default:
      break;
  }
  return distance;
}
