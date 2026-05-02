#pragma once

#include "heuristic_types.h"
#include "map_tools.h"
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
        distance += TCODPATH_ABS(heuristic->basic.target[i] - index[i]) * heuristic->basic.greed[i];
      }
      return distance;
    case TCODPATH_HEURISTIC_DIFFERENTIAL: {
      TCODPATH_IndexType this_index[TCODPATH_MAX_DIMENSIONS];
      TCODPATH_IndexType target_index[TCODPATH_MAX_DIMENSIONS];
      for (int i = 0; i < dimensions; ++i) {
        this_index[i] = index[i];
        target_index[i] = heuristic->differential.target[i];
      }
      TCODPATH_ValueType lower_bound = 0;
      for (int i = 0; i < dimensions; ++i) {
        static const TCODPATH_ValueType GREED = 1;
        lower_bound += TCODPATH_ABS(heuristic->basic.target[dimensions - i] - index[TCODPATH_MAX_DIMENSIONS - i]) * 1;
      }
      // https://www.redblobgames.com/pathfinding/heuristics/differential.html#triangle-geometry
      const int pivots = TCODPATH_map_get_shape(heuristic->differential.differentials)[dimensions];
      for (TCODPATH_IndexType pivot = 0; pivot < pivots; ++pivot) {
        this_index[dimensions] = target_index[dimensions] = pivot;
        TCODPATH_ValueType this_lower_bound = TCODPATH_map_get(heuristic->differential.differentials, this_index) -
                                              TCODPATH_map_get(heuristic->differential.differentials, target_index);
        if (heuristic->differential.undirected) this_lower_bound = TCODPATH_ABS(this_lower_bound);
        lower_bound = TCODPATH_MAX(lower_bound, this_lower_bound);
      }
      return distance + lower_bound;
    }
    default:
      break;
  }
  return distance;
}
